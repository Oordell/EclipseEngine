#include "movement_random.hpp"
#include <random>

namespace bomberman {

MovementRandom::MovementRandom(GetEnemyMetaDataFn get_enemy_data, GetEnemyPositionFn get_enemy_position,
                               SetEnemyDirectionFn set_enemy_direction, SetEnemyPositionFn set_enemy_position,
                               PlayingBoard& playing_board, const MovementSettings& settings)
    : get_enemy_meta_data_(get_enemy_data),
      get_enemy_position_(get_enemy_position),
      set_enemy_direction_(set_enemy_direction),
      set_enemy_position_(set_enemy_position),
      playing_board_(playing_board),
      settings_(settings) {
	current_direction_ = get_new_direction();
}

void MovementRandom::on_update(au::QuantityF<au::Seconds> timestep) {
	const auto current_position = get_enemy_position_();
	if (current_position == target_position_ || !current_direction_.has_value()) {
		current_direction_ = get_new_direction();
	}
	if (!current_direction_.has_value()) {
		return;
	}

	auto next_position = current_position;

	auto x_error = target_position_.x - current_position.x;
	auto y_error = target_position_.y - current_position.y;
	auto delta   = pixels(get_enemy_meta_data_().speed * timestep.in(au::seconds));
	switch (current_direction_.value()) {
		case EnemyDirection::down: {
			if (std::abs(delta.in(pixels)) > std::abs(y_error.in(pixels))) {
				next_position.y = target_position_.y;
			} else {
				next_position.y -= delta;
			}
			break;
		}
		case EnemyDirection::up: {
			if (std::abs(delta.in(pixels)) > std::abs(y_error.in(pixels))) {
				next_position.y = target_position_.y;
			} else {
				next_position.y += delta;
			}
			break;
		}
		case EnemyDirection::left: {
			if (std::abs(delta.in(pixels)) > std::abs(x_error.in(pixels))) {
				next_position.x = target_position_.x;
			} else {
				next_position.x -= delta;
			}
			break;
		}
		case EnemyDirection::right: {
			if (std::abs(delta.in(pixels)) > std::abs(x_error.in(pixels))) {
				next_position.x = target_position_.x;
			} else {
				next_position.x += delta;
			}
			break;
		}
	}

	set_enemy_direction_(current_direction_.value());
	set_enemy_position_(next_position);
}

std::optional<EnemyDirection> MovementRandom::get_new_direction() {
	bool next_tile_is_valid = false;
	EnemyDirection direction;

	const auto current_position    = get_enemy_position_();
	auto current_pose_snap_to_grid = current_position;
	current_pose_snap_to_grid.x    = pixels(std::round(current_pose_snap_to_grid.x.in(pixels)));
	current_pose_snap_to_grid.y    = pixels(std::round(current_pose_snap_to_grid.y.in(pixels)));

	static std::mt19937 rng(std::random_device {}());
	std::uniform_int_distribution<int> dist(0, 3);

	std::unordered_map<EnemyDirection, bool> direction_options_tried = {{EnemyDirection::down, false},
	                                                                    {EnemyDirection::up, false},
	                                                                    {EnemyDirection::left, false},
	                                                                    {EnemyDirection::right, false}};

	while (!next_tile_is_valid) {
		direction                             = static_cast<EnemyDirection>(dist(rng));
		direction_options_tried.at(direction) = true;

		auto next_pose_snap_to_grid = current_pose_snap_to_grid;
		switch (direction) {
			case EnemyDirection::down: {
				next_pose_snap_to_grid.y -= pixels(1.F);
				break;
			}
			case EnemyDirection::up: {
				next_pose_snap_to_grid.y += pixels(1.F);
				break;
			}
			case EnemyDirection::left: {
				next_pose_snap_to_grid.x -= pixels(1.F);
				break;
			}
			case EnemyDirection::right: {
				next_pose_snap_to_grid.x += pixels(1.F);
				break;
			}
		}
		target_position_ = next_pose_snap_to_grid;

		if (playing_board_.is_coordinate_valid(next_pose_snap_to_grid) &&
		    (playing_board_.at(next_pose_snap_to_grid).walkable ||
		     (!playing_board_.at(next_pose_snap_to_grid).tile_content.empty() && settings_.ignore_soft_walls &&
		      playing_board_.at(next_pose_snap_to_grid).tile_content.back()->on_player_interaction() ==
		          CellContent::wall_soft))) {
			next_tile_is_valid = true;
			break;
		}

		bool all_options_tried = true;
		for (auto option_tried : direction_options_tried) {
			all_options_tried &= option_tried.second;
		}
		if (all_options_tried) {
			return std::nullopt;
		}
	}
	return direction;
}

}  // namespace bomberman