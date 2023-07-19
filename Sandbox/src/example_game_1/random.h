#pragma once

#include <random>

class Random {
public:
	static void init() { random_engine_.seed(std::random_device()()); }

	static float get_float() {
		return static_cast<float>(distribution_(random_engine_)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
	}

private:
	static std::mt19937 random_engine_;
	static std::uniform_int_distribution<std::mt19937::result_type> distribution_;
};