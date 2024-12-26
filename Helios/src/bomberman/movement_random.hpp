#pragma once

#include <Eclipse.h>
#include "common_types.hpp"
#include "enemy.hpp"
#include "playing_board.hpp"
#include "movement_interface.hpp"

namespace bomberman {

class MovementRandom : public MovementInterface {
public:
	explicit MovementRandom(GetEnemyMetaDataFn get_enemy_data, GetEnemyPositionFn get_enemy_position,
	                        SetEnemyDirectionFn set_enemy_direction, SetEnemyPositionFn set_enemy_position,
	                        PlayingBoard& playing_board, const MovementSettings& settings);
	~MovementRandom() = default;

	void on_update(au::QuantityF<au::Seconds> timestep) override;

private:
	std::optional<EnemyDirection> get_new_direction();

	GetEnemyMetaDataFn get_enemy_meta_data_;
	GetEnemyPositionFn get_enemy_position_;
	SetEnemyDirectionFn set_enemy_direction_;
	SetEnemyPositionFn set_enemy_position_;
	PlayingBoard& playing_board_;
	MovementSettings settings_;
	eclipse::Point2D target_position_;
	std::optional<EnemyDirection> current_direction_ {std::nullopt};
};

}  // namespace bomberman