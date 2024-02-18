#pragma once

#include "eclipse/core/core.h"

#include <au.hh>

namespace eclipse {

class ECLIPSE_API Timestep {
public:
	explicit Timestep(float time = 0.0F) : time_(au::seconds(time)) {}

	~Timestep() = default;

	auto operator<=>(const Timestep&) const = default;

	operator au::Quantity<au::Seconds, float>() const { return time_; }

	au::Quantity<au::Seconds, float> get() const { return time_; }

	au::Quantity<au::Milli<au::Seconds>, float> get_milliseconds() const { return time_.as(au::milli(au::seconds)); }

private:
	au::Quantity<au::Seconds, float> time_ {};
};

}  // namespace eclipse