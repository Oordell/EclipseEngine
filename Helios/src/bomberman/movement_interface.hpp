#pragma once

#include "Eclipse.h"
#include "enemy_data.hpp"

namespace bomberman {

using GetEnemyMetaDataFn  = std::function<const EnemyMetaData&()>;
using GetEnemyPositionFn  = std::function<const eclipse::Point2D&()>;
using SetEnemyDirectionFn = std::function<void(EnemyDirection)>;
using SetEnemyPositionFn  = std::function<void(const eclipse::Point2D&)>;

struct MovementSettings {
	bool ignore_soft_walls {false};
};

class MovementInterface {
public:
	virtual ~MovementInterface()                                = default;
	virtual void on_update(au::QuantityF<au::Seconds> timestep) = 0;
};

}  // namespace bomberman