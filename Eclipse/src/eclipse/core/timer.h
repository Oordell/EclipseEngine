#pragma once

#include <chrono>
#include <au.hh>

namespace eclipse {

class Timer {
public:
	void reset() { start_ = std::chrono::high_resolution_clock::now(); }

	[[nodiscard]] constexpr au::QuantityF<au::Seconds> elapsed() const {
		return au::nano(au::seconds)(static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(
		                                                    std::chrono::high_resolution_clock::now() - start_)
		                                                    .count()))
		    .as(au::seconds);
	}

	[[nodiscard]] constexpr au::QuantityF<au::Milli<au::Seconds>> elapsed_milliseconds() const {
		return elapsed().as<au::Milli<au::Seconds>>();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start_ = std::chrono::high_resolution_clock::now();
};

}  // namespace eclipse