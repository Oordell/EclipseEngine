#include "ecpch.h"
#include "level.hpp"

namespace bomberman {

Level::Level() {
	enemies_.reserve(30);
	bombs_.reserve(30);
	//	soft_walls_.reserve(100);
}

void Level::on_update(au::QuantityF<au::Seconds> timestep) {
	if (!eclipse::InputManager::is_key_pressed(eclipse::KeyCode::W) &&
	    !eclipse::InputManager::is_key_pressed(eclipse::KeyCode::A) &&
	    !eclipse::InputManager::is_key_pressed(eclipse::KeyCode::S) &&
	    !eclipse::InputManager::is_key_pressed(eclipse::KeyCode::D)) {
		player_.set_player_state(PlayerState::idle);
	} else {
		update_player_position(timestep);
	}
	player_.on_update(timestep);
	update_enemies(timestep);
	update_playing_board(timestep);
	update_bombs(timestep);
	update_bomb_rays(timestep);
	check_if_player_touched_by_enemy();
	check_collisions_with_bombs_and_rays();
}

void Level::on_render() {}

void Level::on_imgui_render() {}

void Level::on_event(eclipse::Event& event) {
	eclipse::EventDispatcher dispatcher(event);
	dispatcher.dispatch<eclipse::KeyPressedEvent>(EC_BIND_EVENT_FN(Level::on_key_pressed));
}

void Level::set_context(const eclipse::ref<eclipse::Scene>& context) {
	context_ = context;
	player_.set_context(context);
}

void Level::setup_level() {
	using enum CellContent;

	playing_board_ = PlayingBoard(std::filesystem::path("src/bomberman/levels/level1.txt"),
	                              texture_sheet_maps_enemies_bombs_items_, context_);

	player_.create_player();
	player_.set_player_position(playing_board_.get_player_start_coordinates());

	for (const auto& enemy : playing_board_.get_enemies()) {
		switch (enemy.type) {
			case (EnemyType::enemy_red_balloon): {
				add_enemy<EnemyRedBalloon>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_duck): {
				add_enemy<EnemyGreenDuck>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_frog): {
				add_enemy<EnemyBlueFrog>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_bat): {
				add_enemy<EnemyBlueBat>(enemy.position);
				break;
			}
			case (EnemyType::enemy_white_ghost): {
				add_enemy<EnemyWhiteGhost>(enemy.position);
				break;
			}
			case (EnemyType::enemy_red_flower): {
				add_enemy<EnemyRedFlower>(enemy.position);
				break;
			}
			case (EnemyType::enemy_red_bear): {
				add_enemy<EnemyRedBear>(enemy.position);
				break;
			}
			case (EnemyType::enemy_red_coin): {
				add_enemy<EnemyRedCoin>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_fish): {
				add_enemy<EnemyGreenFish>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_cloud): {
				add_enemy<EnemyBlueCloud>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_fog): {
				add_enemy<EnemyBlueFog>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_smiley_bouncer): {
				add_enemy<EnemyGreenSmileyBouncer>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_croc): {
				add_enemy<EnemyGreenCroc>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_jellyfish): {
				add_enemy<EnemyBlueJellyfish>(enemy.position);
				break;
			}
			case (EnemyType::enemy_blue_spinning_top): {
				add_enemy<EnemyBlueSpinningTop>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_frog): {
				add_enemy<EnemyGreenFrog>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_bomb): {
				add_enemy<EnemyGreenBomb>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_white_centipede): {
				add_enemy<EnemyGreenWhiteCentipede>(enemy.position);
				break;
			}
			case (EnemyType::enemy_green_bold_roller): {
				add_enemy<EnemyGreenBoldRoller>(enemy.position);
				break;
			}
		}
	}

	auto level_type   = eclipse::units::pixels(playing_board_.get_level_type());
	background_grass_ = eclipse::make_ref<eclipse::SubTexture2D>(eclipse::SubTexture2DProperties {
	    .texture_sheet = texture_sheet_maps_enemies_bombs_items_,
	    .tile_index_x  = eclipse::units::pixels(2) + level_type * details::texture_sheet_level_separator_,
	    .tile_index_y  = eclipse::units::pixels(22),
	    .tile_width    = eclipse::units::pixels(1),
	    .tile_height   = eclipse::units::pixels(1)});

	eclipse::Entity grass_background_entity = context_->create_entity("grass");
	grass_background_entity.add_component<eclipse::component::SubTexture>(background_grass_);

	grass_background_entity.get_component<eclipse::component::Transform>().translation.x =
	    static_cast<float>((playing_board_.get_width() - 1) / 2);
	grass_background_entity.get_component<eclipse::component::Transform>().translation.y =
	    static_cast<float>((playing_board_.get_height() - 1) / 2);
	grass_background_entity.get_component<eclipse::component::Transform>().translation.z = details::Z_GRASS;

	grass_background_entity.get_component<eclipse::component::Transform>().scale.x =
	    static_cast<float>(playing_board_.get_width());
	grass_background_entity.get_component<eclipse::component::Transform>().scale.y =
	    static_cast<float>(playing_board_.get_height());
}

void Level::update_player_position(au::QuantityF<au::Seconds> timestep) {
	if (player_.get_player_status() != PlayerStatus::alive) {
		return;
	}

	auto delta                     = pixels(player_.get_player_speed() * timestep.in(au::seconds));
	auto current_pose_snap_to_grid = player_.get_player_position();
	current_pose_snap_to_grid.x    = pixels(std::round(current_pose_snap_to_grid.x.in(pixels)));
	current_pose_snap_to_grid.y    = pixels(std::round(current_pose_snap_to_grid.y.in(pixels)));

	bool current_tile_is_valid = false;
	if (playing_board_.is_coordinate_valid(current_pose_snap_to_grid) &&
	    playing_board_.at(current_pose_snap_to_grid).walkable) {
		current_tile_is_valid = true;
	}

	auto next_pose_snap_to_grid = current_pose_snap_to_grid;
	switch (player_.get_player_direction()) {
		case PlayerDirection::down: {
			next_pose_snap_to_grid.y -= pixels(1.F);
			break;
		}
		case PlayerDirection::up: {
			next_pose_snap_to_grid.y += pixels(1.F);
			break;
		}
		case PlayerDirection::left: {
			next_pose_snap_to_grid.x -= pixels(1.F);
			break;
		}
		case PlayerDirection::right: {
			next_pose_snap_to_grid.x += pixels(1.F);
			break;
		}
	}

	bool next_tile_is_valid = false;
	if (playing_board_.is_coordinate_valid(next_pose_snap_to_grid) && playing_board_.at(next_pose_snap_to_grid).walkable) {
		next_tile_is_valid = true;
	}

	auto current_pose = player_.get_player_position();

	switch (player_.get_player_direction()) {
		case PlayerDirection::down: {
			current_pose.y -= delta;
			break;
		}
		case PlayerDirection::up: {
			current_pose.y += delta;
			break;
		}
		case PlayerDirection::left: {
			current_pose.x -= delta;
			break;
		}
		case PlayerDirection::right: {
			current_pose.x += delta;
			break;
		}
	}

	static constexpr float threshold = 0.25F;

	switch (player_.get_player_direction()) {
		case PlayerDirection::down:
		case PlayerDirection::up: {
			auto x_error = current_pose_snap_to_grid.x - current_pose.x;
			current_pose.x += (std::abs(x_error.in(pixels)) < std::abs(delta.in(pixels)))
			                      ? x_error
			                      : pixels(std::copysign(delta.in(pixels), x_error.in(pixels)));
			break;
		}
		case PlayerDirection::left:
		case PlayerDirection::right: {
			auto y_error = current_pose_snap_to_grid.y - current_pose.y;
			current_pose.y += (std::abs(y_error.in(pixels)) < std::abs(delta.in(pixels)))
			                      ? y_error
			                      : pixels(std::copysign(delta.in(pixels), y_error.in(pixels)));
			break;
		}
	}

	if (next_tile_is_valid || is_walkable(current_pose)) {
		player_.set_player_position(current_pose);
	}

	on_item_pickup();
}

bool Level::on_key_pressed(eclipse::KeyPressedEvent& event) {
	switch (event.get_key_code()) {
		case eclipse::KeyCode::W: {
			player_.set_player_state(PlayerState::running);
			player_.set_player_direction(PlayerDirection::up);
			break;
		}
		case eclipse::KeyCode::A: {
			player_.set_player_state(PlayerState::running);
			player_.set_player_direction(PlayerDirection::left);
			break;
		}
		case eclipse::KeyCode::S: {
			player_.set_player_state(PlayerState::running);
			player_.set_player_direction(PlayerDirection::down);
			break;
		}
		case eclipse::KeyCode::D: {
			player_.set_player_state(PlayerState::running);
			player_.set_player_direction(PlayerDirection::right);
			break;
		}
		case eclipse::KeyCode::space: {
			place_bomb();
			break;
		}
		default:
			break;
	}

	return false;
}

bool Level::is_walkable(const eclipse::Point2D& position) const {
	static constexpr auto player_padding_x = pixels(0.49F);
	static constexpr auto player_padding_y = pixels(0.49F);

	if (!playing_board_.at(position).walkable) {
		return false;
	}

	auto top_left     = position;
	auto top_right    = position;
	auto bottom_left  = position;
	auto bottom_right = position;

	top_left.x -= player_padding_x;
	top_left.y += player_padding_y;

	top_right.x += player_padding_x;
	top_right.y += player_padding_y;

	bottom_left.x -= player_padding_x;
	bottom_left.y -= player_padding_y;

	bottom_right.x += player_padding_x;
	bottom_right.y -= player_padding_y;

	return playing_board_.at(top_left).walkable && playing_board_.at(top_right).walkable &&
	       playing_board_.at(bottom_left).walkable && playing_board_.at(bottom_right).walkable;
}

void Level::place_bomb() {
	if (bombs_.size() >= player_.get_total_num_of_bombs()) {
		return;
	}
	auto current_position = player_.get_player_position();
	playing_board_.clamp_coordinates(current_position);
	if (!position_contains_bomb(current_position)) {
		bombs_.emplace_back(texture_sheet_maps_enemies_bombs_items_, current_position, context_,
		                    EC_BIND_EVENT_FN(Level::on_bomb_explosion), player_.get_bomb_reach());
		playing_board_[current_position].walkable = false;
	}
}

bool Level::position_contains_bomb(const eclipse::Point2D& position) const {
	for (const auto& b : bombs_) {
		if (b.get_position() == position) {
			return true;
		}
	}
	return false;
}

void Level::update_enemies(au::QuantityF<au::Seconds> timestep) {
	for (auto i = enemies_.begin(); i != enemies_.end();) {
		(*i)->on_update(timestep);
		if ((*i)->has_expired()) {
			i = enemies_.erase(i);
		} else {
			++i;
		}
	}
}

void Level::update_playing_board(au::QuantityF<au::Seconds> timestep) {
	for (int i = 0; i < playing_board_.size(); i++) {
		for (auto cell = playing_board_[i].tile_content.begin(); cell != playing_board_[i].tile_content.end();) {
			(*cell)->on_update(timestep);
			if ((*cell)->has_expired()) {
				playing_board_[i].walkable = true;
				(*cell)->destroy();
				cell = playing_board_[i].tile_content.erase(cell);
			} else {
				++cell;
			}
		}
	}
}

void Level::update_bombs(au::QuantityF<au::Seconds> timestep) {
	for (auto i = bombs_.begin(); i != bombs_.end();) {
		i->on_update(timestep);
		if (i->has_expired()) {
			playing_board_[playing_board_.get_index(i->get_position())].walkable = true;
			i->destroy();
			if (bombs_.size() > 1) {
				i = bombs_.erase(i);
			} else {
				bombs_.erase(i);
			}
		} else {
			++i;
		}
		if (bombs_.empty()) {
			break;
		}
	}
}

void Level::update_bomb_rays(au::QuantityF<au::Seconds> timestep) {
	for (auto i = bomb_rays_.begin(); i != bomb_rays_.end();) {
		i->on_update(timestep);
		if (i->has_expired()) {
			i->destroy();
			if (bomb_rays_.size() > 1) {
				i = bomb_rays_.erase(i);
			} else {
				bomb_rays_.erase(i);
			}
		} else {
			++i;
		}
		if (bomb_rays_.empty()) {
			break;
		}
	}
}

void Level::check_collisions_with_bombs_and_rays() {
	auto player_position = player_.get_player_position();
	playing_board_.clamp_coordinates(player_position);
	for (const auto& bomb_ray : bomb_rays_) {
		if (player_position == bomb_ray.get_position()) {
			player_.on_bomb_ray_hit();
		}
		for (const auto& enemy : enemies_) {
			auto enemy_position = enemy->get_position();
			playing_board_.clamp_coordinates(enemy_position);
			if (enemy_position == bomb_ray.get_position()) {
				enemy->on_bomb_ray_hit();
			}
		}
	}
	for (const auto& bomb : bombs_) {
		if (!bomb.bomb_is_exploding()) {
			continue;
		}
		if (player_position == bomb.get_position()) {
			player_.on_bomb_ray_hit();
		}
		for (const auto& enemy : enemies_) {
			auto enemy_position = enemy->get_position();
			playing_board_.clamp_coordinates(enemy_position);
			if (enemy_position == bomb.get_position()) {
				enemy->on_bomb_ray_hit();
			}
		}
	}
}

void Level::on_bomb_explosion(const eclipse::Point2D& position, uint32_t reach) {
	on_bomb_ray_collision(position);
	playing_board_[position].walkable = true;
	BombRayDescription initial_ray_state;
	initial_ray_state.section     = reach == 1 ? BombRaySection::end : BombRaySection::midt;
	eclipse::Point2D ray_position = position;

	bool left_blocked  = false;
	bool right_blocked = false;
	bool up_blocked    = false;
	bool down_blocked  = false;

	// Mid sections:
	for (uint32_t i = 1; i <= reach; i++) {
		if (i >= reach) {
			initial_ray_state.section = BombRaySection::end;
		}

		if (!left_blocked) {
			initial_ray_state.direction = BombRayDirection::left;
			ray_position.x              = position.x - pixels(i);
			left_blocked                = is_position_ray_blocking(ray_position);
			on_bomb_ray_collision(ray_position);
			if (!left_blocked && !does_position_contain_bomb_ray(ray_position) && !position_contains_bomb(ray_position)) {
				bomb_rays_.emplace_back(texture_sheet_maps_enemies_bombs_items_, ray_position, context_, initial_ray_state);
				on_bomb_ray_created(ray_position);
			}
			ray_position = position;
		}

		if (!right_blocked) {
			initial_ray_state.direction = BombRayDirection::right;
			ray_position.x              = position.x + pixels(i);
			right_blocked               = is_position_ray_blocking(ray_position);
			on_bomb_ray_collision(ray_position);
			if (!right_blocked && !does_position_contain_bomb_ray(ray_position) && !position_contains_bomb(ray_position)) {
				bomb_rays_.emplace_back(texture_sheet_maps_enemies_bombs_items_, ray_position, context_, initial_ray_state);
				on_bomb_ray_created(ray_position);
			}
			ray_position = position;
		}

		if (!up_blocked) {
			initial_ray_state.direction = BombRayDirection::up;
			ray_position.y              = position.y + pixels(i);
			up_blocked                  = is_position_ray_blocking(ray_position);
			on_bomb_ray_collision(ray_position);
			if (!up_blocked && !does_position_contain_bomb_ray(ray_position) && !position_contains_bomb(ray_position)) {
				bomb_rays_.emplace_back(texture_sheet_maps_enemies_bombs_items_, ray_position, context_, initial_ray_state);
				on_bomb_ray_created(ray_position);
			}
			ray_position = position;
		}

		if (!down_blocked) {
			initial_ray_state.direction = BombRayDirection::down;
			ray_position.y              = position.y - pixels(i);
			down_blocked                = is_position_ray_blocking(ray_position);
			on_bomb_ray_collision(ray_position);
			if (!down_blocked && !does_position_contain_bomb_ray(ray_position) && !position_contains_bomb(ray_position)) {
				bomb_rays_.emplace_back(texture_sheet_maps_enemies_bombs_items_, ray_position, context_, initial_ray_state);
				on_bomb_ray_created(ray_position);
			}
			ray_position = position;
		}
	}
}

void Level::on_bomb_ray_created(const eclipse::Point2D& position) {}

bool Level::is_position_ray_blocking(const eclipse::Point2D& position) const {
	if (!playing_board_.is_coordinate_valid(position)) {
		return true;
	}
	return !playing_board_.at(position).walkable;
}

bool Level::does_position_contain_bomb_ray(const eclipse::Point2D& position) const {
	for (const auto& ray : bomb_rays_) {
		if (ray.get_position() == position) {
			return true;
		}
	}
	return false;
}

void Level::on_bomb_ray_collision(const eclipse::Point2D& position) {
	if (!playing_board_.at(position).tile_content.empty()) {
		playing_board_.at(position).tile_content.back()->on_bomb_ray_hit();
	}
	auto player_position = player_.get_player_position();
	playing_board_.clamp_coordinates(player_position);
	if (position == player_position) {
		player_.on_bomb_ray_hit();
	}
	for (const auto& enemy : enemies_) {
		auto enemy_position = enemy->get_position();
		playing_board_.clamp_coordinates(enemy_position);
		if (enemy_position == position) {
			enemy->on_bomb_ray_hit();
		}
	}
	for (auto& bomb : bombs_) {
		if (bomb.get_position() == position && bomb.bomb_is_placed()) {
			bomb.on_bomb_ray_hit();
		}
	}
}

void Level::on_item_pickup() {
	const auto& player_position = player_.get_player_position();
	if (!playing_board_.at(player_position).tile_content.empty()) {
		auto interaction_type = playing_board_.at(player_position).tile_content.back()->on_player_interaction();
		switch (interaction_type) {
			case CellContent::item_bomb_increment: {
				player_.increase_total_number_of_bombs();
				break;
			};
			case CellContent::item_bomb_reach: {
				player_.increase_bomb_reach();
				break;
			}
			case CellContent::item_speed_boost: {
				player_.speed_boost();
				break;
			}
			case CellContent::item_invincibility: {
				player_.invincible();
				break;
			}
			default: {
				break;
			};
		}
	}
}

void Level::check_if_player_touched_by_enemy() {
	auto player_position = player_.get_player_position();
	playing_board_.clamp_coordinates(player_position);
	for (const auto& enemy : enemies_) {
		if (enemy->is_dying()) {
			continue;
		}
		auto enemy_position = enemy->get_position();
		playing_board_.clamp_coordinates(enemy_position);
		if (enemy_position == player_position) {
			player_.on_enemy_collision();
			return;
		}
	}
}

}  // namespace bomberman