#pragma once

#include "eclipse/common_types/strong_types.h"

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

namespace eclipse::debug {

struct ProfileResult {
	std::string name;
	long long start_time;
	long long end_time;
	uint32_t thread_id;
};

struct InstrumentationSession {
	std::string name;
};

class Instrumentor {
public:
	Instrumentor() : current_session_(nullptr), profile_count_(0) {}

	void begin_session(const std::string& name, FilePath filepath = FilePath("results.json")) {
		output_stream_.open(filepath.value());
		write_header();
		current_session_ = new InstrumentationSession {name};
	}

	void end_session() {
		write_footer();
		output_stream_.close();
		delete current_session_;
		current_session_ = nullptr;
		profile_count_   = 0;
	}

	void write_profile(const ProfileResult& result) {
		if (profile_count_++ > 0) {
			output_stream_ << ",";
		}

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		output_stream_ << "{";
		output_stream_ << "\"cat\":\"function\",";
		output_stream_ << "\"dur\":" << (result.end_time - result.start_time) << ',';
		output_stream_ << "\"name\":\"" << name << "\",";
		output_stream_ << "\"ph\":\"X\",";
		output_stream_ << "\"pid\":0,";
		output_stream_ << "\"tid\":" << result.thread_id << ",";
		output_stream_ << "\"ts\":" << result.start_time;
		output_stream_ << "}";

		output_stream_.flush();
	}

	void write_header() {
		output_stream_ << "{\"otherData\": {},\"traceEvents\":[";
		output_stream_.flush();
	}

	void write_footer() {
		output_stream_ << "]}";
		output_stream_.flush();
	}

	static Instrumentor& get() {
		static Instrumentor instance;
		return instance;
	}

private:
	InstrumentationSession* current_session_;
	std::ofstream output_stream_;
	int profile_count_;
};

class InstrumentationTimer {
public:
	using high_resolution_clock = std::chrono::high_resolution_clock;
	using microseconds          = std::chrono::microseconds;

	InstrumentationTimer(const char* name)
	    : name_(name), start_time_point_(high_resolution_clock::now()), running_(true) {}

	~InstrumentationTimer() {
		if (running_) {
			stop();
		}
	}

	void stop() {
		auto end_time_point = high_resolution_clock::now();
		long long start     = std::chrono::time_point_cast<microseconds>(start_time_point_).time_since_epoch().count();
		long long end       = std::chrono::time_point_cast<microseconds>(end_time_point).time_since_epoch().count();
		uint32_t thread_id  = static_cast<uint32_t>(std::hash<std::thread::id> {}(std::this_thread::get_id()));
		Instrumentor::get().write_profile({name_, start, end, thread_id});
		running_ = false;
	}

private:
	const char* name_;
	std::chrono::time_point<high_resolution_clock> start_time_point_;
	bool running_;
};

}  // namespace eclipse::debug

#define EC_PROFILE 1

/* clang-format off */
#if EC_PROFILE
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