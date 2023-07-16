#pragma once

#include "eclipse/common_types/file_path.h"

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

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
	Instrumentor() : current_session_(nullptr) {}

	void begin_session(const std::string& name, FilePath filepath = FilePath("results.json")) {
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

		output_stream_.open(filepath.value());
		if (output_stream_.is_open()) {
			current_session_ = new InstrumentationSession {name};
			write_header();
		} else {
			if (Log::get_core_logger()) {  // Edge case: begin_session() might be before Log::init()
				EC_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath.value());
			}
		}
	}

	void end_session() {
		std::lock_guard lock(mutex_);
		internal_end_session();
	}

	void write_profile(const ProfileResult& result) {
		std::stringstream json;

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsed_time.count()) << ',';
		json << "\"name\":\"" << name << "\",";
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

}  // namespace eclipse::debug

/* clang-format off */
#if EC_PROFILE_ENABLED
	#define EC_PROFILE_BEGIN_SESSION(name, filepath) ::eclipse::debug::Instrumentor::get().begin_session(name, filepath)
	#define EC_PROFILE_END_SESSION()                 ::eclipse::debug::Instrumentor::get().end_session()
	#define EC_PROFILE_SCOPE(name)                   ::eclipse::debug::InstrumentationTimer timer##__LINE__(name)
	#define EC_PROFILE_FUNCTION()                    EC_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define EC_PROFILE_BEGIN_SESSION(name, filepath)
	#define EC_PROFILE_END_SESSION()
	#define EC_PROFILE_SCOPE(name)
	#define EC_PROFILE_FUNCTION()
#endif
/* clang-format on */