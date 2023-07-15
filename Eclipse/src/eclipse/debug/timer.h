#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/profile_results.h"

#include <chrono>
#include <iostream>

namespace eclipse {

template <typename Fn>
class ECLIPSE_API Timer {
public:
	using high_resolution_clock = std::chrono::high_resolution_clock;
	using milliseconds          = std::chrono::milliseconds;
	using nanoseconds           = std::chrono::nanoseconds;
	using steady_clock          = std::chrono::steady_clock;

	explicit Timer(const char* name, Fn&& callback) : name_(name), callback_func_(callback), running_(true) {}

	~Timer() {
		if (running_) {
			stop();
		}
	}

	void stop() {
		auto end_time_point = high_resolution_clock::now();
		long long start_ms  = std::chrono::time_point_cast<nanoseconds>(start_time_point_).time_since_epoch().count();
		long long end_ms    = std::chrono::time_point_cast<nanoseconds>(end_time_point).time_since_epoch().count();
		duration_ms_        = static_cast<float>(end_ms - start_ms) * NANO_TO_MILLI_;
		running_            = false;
		callback_func_({name_, duration_ms_});
	}

private:
	const char* name_;
	Fn callback_func_;
	bool running_;
	std::chrono::time_point<steady_clock> start_time_point_ = high_resolution_clock::now();
	static constexpr float NANO_TO_MILLI_                   = 0.000001F;
	float duration_ms_                                      = 0.0F;
};

}  // namespace eclipse