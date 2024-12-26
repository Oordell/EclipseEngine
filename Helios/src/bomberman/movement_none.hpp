#pragma once

#include <Eclipse.h>
#include "movement_interface.hpp"

namespace bomberman {

class MovementNone : public MovementInterface {
public:
	explicit MovementNone() = default;
	~MovementNone()         = default;

	void on_update(au::QuantityF<au::Seconds> timestep) override {}
};

}  // namespace bomberman