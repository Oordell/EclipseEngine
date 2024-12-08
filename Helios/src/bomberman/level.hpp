#pragma once

#include <au.hh>
#include <Eclipse.h>
#include "player.hpp"
#include "playing_board.hpp"
#include "item.hpp"
#include "enemy.hpp"
#include "soft_wall.hpp"
#include "solid_wall.hpp"
#include "bomb.hpp"
#include "bomb_ray.hpp"

namespace bomberman {

class Level {
public:
	explicit Level();
	~Level() = default;
	void on_update(au::QuantityF<au::Seconds> timestep);
	void on_render();
	void on_imgui_render();
	void on_event(eclipse::Event& event);
	void set_context(const eclipse::ref<eclipse::Scene>& context);
	void setup_level();

	float& get_player_speed() { return player_.get_player_speed(); }

	[[nodiscard]] uint32_t get_bomb_reach() const { return player_.get_bomb_reach(); }

	[[nodiscard]] uint32_t get_total_num_of_bombs() const { return player_.get_total_num_of_bombs(); }

	[[nodiscard]] const eclipse::Point2D& get_player_position() const { return player_.get_player_position(); }

private:
	void update_player_position(au::QuantityF<au::Seconds> timestep);
	bool on_key_pressed(eclipse::KeyPressedEvent& event);
	[[nodiscard]] bool is_walkable(const eclipse::Point2D& position) const;
	void place_bomb();
	[[nodiscard]] bool position_contains_bomb(const eclipse::Point2D& position) const;
	void update_enemies(au::QuantityF<au::Seconds> timestep);
	void update_playing_board(au::QuantityF<au::Seconds> timestep);
	void update_bombs(au::QuantityF<au::Seconds> timestep);
	void update_bomb_rays(au::QuantityF<au::Seconds> timestep);
	void on_bomb_explosion(const eclipse::Point2D& position, uint32_t reach);
	void on_bomb_ray_created(const eclipse::Point2D& position);
	[[nodiscard]] bool is_position_ray_blocking(const eclipse::Point2D& position) const;
	[[nodiscard]] bool does_position_contain_bomb_ray(const eclipse::Point2D& position) const;
	void on_bomb_ray_collision(const eclipse::Point2D& position);
	void on_item_pickup();
	void check_if_player_touched_by_enemy();

	template <typename EnemyType>
	void add_enemy(const eclipse::Point2D& position) {
		enemies_.push_back(eclipse::make_scope<EnemyType>(texture_sheet_, position, context_));
	}

	eclipse::ref<eclipse::TextureSheet> texture_sheet_items_ = eclipse::make_ref<eclipse::TextureSheet>(
	    eclipse::TextureSheetProperties {.texture         = eclipse::Texture2D::create("assets/games/bomberman/items.png"),
	                                     .sub_tile_width  = eclipse::units::pixels(128),
	                                     .sub_tile_height = eclipse::units::pixels(128),
	                                     .sub_tile_spacing_x = eclipse::units::pixels(0),
	                                     .sub_tile_spacing_y = eclipse::units::pixels(0)});
	eclipse::ref<eclipse::TextureSheet> texture_sheet_frames_and_bombs_ =
	    eclipse::make_ref<eclipse::TextureSheet>(eclipse::TextureSheetProperties {
	        .texture            = eclipse::Texture2D::create("assets/games/bomberman/frames_and_bombs2.png"),
	        .sub_tile_width     = eclipse::units::pixels(16),
	        .sub_tile_height    = eclipse::units::pixels(16),
	        .sub_tile_spacing_x = eclipse::units::pixels(0),
	        .sub_tile_spacing_y = eclipse::units::pixels(0)});
	eclipse::ref<eclipse::TextureSheet> texture_sheet_player_ = eclipse::make_ref<eclipse::TextureSheet>(
	    eclipse::TextureSheetProperties {.texture = eclipse::Texture2D::create("assets/games/bomberman/player2.png"),
	                                     .sub_tile_width     = eclipse::units::pixels(24),
	                                     .sub_tile_height    = eclipse::units::pixels(24),
	                                     .sub_tile_spacing_x = eclipse::units::pixels(0),
	                                     .sub_tile_spacing_y = eclipse::units::pixels(0)});
	eclipse::ref<eclipse::TextureSheet> texture_sheet_ = eclipse::make_ref<eclipse::TextureSheet>(
	    eclipse::TextureSheetProperties {.texture = eclipse::Texture2D::create("assets/games/bomberman/tilesheet.png"),
	                                     .sub_tile_width     = eclipse::units::pixels(16),
	                                     .sub_tile_height    = eclipse::units::pixels(16),
	                                     .sub_tile_spacing_x = eclipse::units::pixels(0),
	                                     .sub_tile_spacing_y = eclipse::units::pixels(0)});
	eclipse::ref<eclipse::SubTexture2D> background_grass_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = eclipse::units::pixels(7),
	                                     .tile_index_y  = eclipse::units::pixels(22),
	                                     .tile_width    = eclipse::units::pixels(1),
	                                     .tile_height   = eclipse::units::pixels(1),
	                                     .offset_x      = eclipse::units::pixels(0),
	                                     .offset_y      = eclipse::units::pixels(0)});

	eclipse::ref<eclipse::Scene> context_;
	Player player_ {texture_sheet_player_};
	std::vector<eclipse::scope<Enemy>> enemies_;
	std::vector<Bomb> bombs_;
	std::vector<BombRay> bomb_rays_;
	PlayingBoard playing_board_;
};
}  // namespace bomberman