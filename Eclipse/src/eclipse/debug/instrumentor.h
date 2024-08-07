#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <filesystem>

namespace eclipse::debug {

using floating_point_microseconds = std::chrono::duration<double, std::micro>;
using steady_clock                = std::chrono::steady_clock;
using microseconds                = std::chrono::microseconds;

struct ProfileResult {
	std::string name;
	floating_point_microseconds start_time;
	microseconds elapsed_time;
	std::thread::id thread_id;
};

struct InstrumentationSession {
	std::string name;
};

class Instrumentor {
public:
	Instrumentor(const Instrumentor&) = delete;
	Instrumentor(Instrumentor&&)      = delete;

	void begin_session(const std::string& name, std::filesystem::path filepath = std::filesystem::path("results.json")) {
		std::lock_guard lock(mutex_);
		if (current_session_) {
			// If there is already a current session, then close it before beginning new one. Subsequent profiling output meant
			// for the original session will end up in newly opened session instead. That's better than having badly profiling
			// output.
			if (Log::get_core_logger()) {  // Edge case: begin_session() might be before Log::init()
				EC_CORE_ERROR("Instrumentor::begin_session('{0}') when session '{1}' already open.", name, current_session_->name);
			}
			internal_end_session();
		}

		output_stream_.open(filepath.string());
		if (output_stream_.is_open()) {
			current_session_ = new InstrumentationSession {name};
			write_header();
		} else {
			if (Log::get_core_logger()) {  // Edge case: begin_session() might be before Log::init()
				EC_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath.string());
			}
		}
	}

	void end_session() {
		std::lock_guard lock(mutex_);
		internal_end_session();
	}

	void write_profile(const ProfileResult& result) {
		std::stringstream json;

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsed_time.count()) << ',';
		json << "\"name\":\"" << result.name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.thread_id << ",";
		json << "\"ts\":" << result.start_time.count();
		json << "}";

		std::lock_guard lock(mutex_);
		if (current_session_) {
			output_stream_ << json.str();
			output_stream_.flush();
		}
	}

	static Instrumentor& get() {
		static Instrumentor instance;
		return instance;
	}

private:
	Instrumentor() : current_session_(nullptr) {}

	~Instrumentor() { end_session(); }

	void write_header() {
		output_stream_ << "{\"otherData\": {},\"traceEvents\":[{}";
		output_stream_.flush();
	}

	void write_footer() {
		output_stream_ << "]}";
		output_stream_.flush();
	}

	// Note: you must already own lock on m_Mutex before calling internal_end_session()
	void internal_end_session() {
		if (current_session_) {
			write_footer();
			output_stream_.close();
			delete current_session_;
			current_session_ = nullptr;
		}
	}

	std::mutex mutex_;
	InstrumentationSession* current_session_;
	std::ofstream output_stream_;
};

class InstrumentationTimer {
public:
	InstrumentationTimer(const char* name) : name_(name), start_time_point_(steady_clock::now()), running_(true) {}

	~InstrumentationTimer() {
		if (running_) {
			stop();
		}
	}

	void stop() {
		auto end_time_point      = steady_clock::now();
		auto high_res_start_time = floating_point_microseconds {start_time_point_.time_since_epoch()};
		auto elapsed_time        = std::chrono::time_point_cast<microseconds>(end_time_point).time_since_epoch() -
		                    std::chrono::time_point_cast<microseconds>(start_time_point_).time_since_epoch();
		Instrumentor::get().write_profile({.name         = name_,
		                                   .start_time   = high_res_start_time,
		                                   .elapsed_time = elapsed_time,
		                                   .thread_id    = std::this_thread::get_id()});
		running_ = false;
	}

private:
	const char* name_;
	std::chrono::time_point<steady_clock> start_time_point_;
	bool running_;
};

namespace utils {

template <size_t N>
struct ChangeResult {
	char data[N];
};

template <size_t N, size_t K>
constexpr auto cleanup_output_string(const char (&expr)[N], const char (&remove)[K]) {
	ChangeResult<N> result = {};

	size_t source_index      = 0;
	size_t destination_index = 0;
	while (source_index < N) {
		size_t match_index = 0;
		while (match_index < K - 1 && source_index + match_index < N - 1 &&
		       expr[source_index + match_index] == remove[match_index]) {
			match_index++;
		}
		if (match_index == K - 1) {
			source_index += match_index;
		}
		result.data[destination_index++] = expr[source_index] == '"' ? '\'' : expr[source_index];
		source_index++;
	}
	return result;
}

}  // namespace utils
}  // namespace eclipse::debug

/* clang-format off */
#if EC_PROFILE_ENABLED
   // Resolve which function signature macro will be used. Note that this only is resolved when the (pre)compiler
   // starts, so the syntax highlighting could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
		defined(__ghs__)
		#define EC_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define EC_FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define EC_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define EC_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define EC_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define EC_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define EC_FUNC_SIG __func__
	#else
		#define EC_FUNC_SIG "EC_FUNC_SIG unknown!"
	#endif

	#define EC_PROFILE_BEGIN_SESSION(name, filepath) ::eclipse::debug::Instrumentor::get().begin_session(name, filepath)
	#define EC_PROFILE_END_SESSION()                 ::eclipse::debug::Instrumentor::get().end_session()
	#define EC_PROFILE_SCOPE_LINE2(name, line)                                                          \
		constexpr auto fixedName##line = ::eclipse::debug::utils::cleanup_output_string(name, "__cdecl "); \
		::eclipse::debug::InstrumentationTimer timer##line(fixedName##line.data)
	#define EC_PROFILE_SCOPE_LINE(name, line) EC_PROFILE_SCOPE_LINE2(name, line)
	#define EC_PROFILE_SCOPE(name)            EC_PROFILE_SCOPE_LINE(name, __LINE__)
	#define EC_PROFILE_FUNCTION() EC_PROFILE_SCOPE(EC_FUNC_SIG)
#else
	#define EC_PROFILE_BEGIN_SESSION(name, filepath)
	#define EC_PROFILE_END_SESSION()
	#define EC_PROFILE_SCOPE(name)
	#define EC_PROFILE_FUNCTION()
#endif
/* clang-format on */