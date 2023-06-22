#pragma once

#include "eclipse/core/core.h"

namespace eclipse {

class ECLIPSE_API Timestep {
public:
	explicit Timestep(float time = 0.0F) : time_(time) {}

	~Timestep() = default;

	auto operator<=>(const Timestep&) const = default;

	operator float() const { return time_; }

	float get_seconds() const { return time_; }

	float get_milliseconds() const { return time_ * SEC_TO_MILLISEC; }

private:
	float time_ {};

	static constexpr float SEC_TO_MILLISEC = 1000.0F;
};

}  // namespace eclipse