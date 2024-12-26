#pragma once

#include "common_types.hpp"

namespace bomberman {

enum class EnemyDirection { left, right, up, down };
enum class EnemyState { alive, dead, expired };
enum class EnemySpriteAliveAnimation { first, second, third, forth };
enum class EnemySpriteDeadAnimation { first, second, third, forth, fifth, sixth };
enum class EnemyMovementType { none, random, state_based };
enum class EnemyMovementState { stationary, random, chaising, fleeing };

struct EnemySprite {
	EnemyDirection direction {EnemyDirection::left};
	EnemySpriteAliveAnimation animation {EnemySpriteAliveAnimation::first};

	void flip_direction() {
		if (direction == EnemyDirection::left) {
			direction = EnemyDirection::right;
		} else if (direction == EnemyDirection::right) {
			direction = EnemyDirection::up;
		} else if (direction == EnemyDirection::up) {
			direction = EnemyDirection::down;
		} else if (direction == EnemyDirection::down) {
			direction = EnemyDirection::left;
		}
	}

	auto operator<=>(const EnemySprite&) const = default;
};

struct EnemyMetaData {
	EnemyType enemy_type;
	EnemyMovementType movement_type {EnemyMovementType::none};
	EnemyMovementState movement_state {EnemyMovementState::stationary};
	float speed {1.F};
	bool ignore_soft_walls {false};
};

}  // namespace bomberman