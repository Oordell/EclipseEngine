#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

enum class PlayerState { idle, running };
enum class PlayerStatus { alive, dead, game_over };
enum class PlayerDirection { up, down, left, right };
enum class DeadPlayerAnimation { first, second, third, forth, fifth, sixth, seventh, eight };
enum class PlayerColor : uint32_t {
	white     = 0,
	black     = 1,
	red       = 2,
	yellow    = 3,
	green     = 4,
	turquoise = 5,
	blue      = 6,
	pink      = 7
};

struct PlayerStateDescription {
	PlayerStatus status {PlayerStatus::alive};
	PlayerState state {PlayerState::idle};
	PlayerDirection direction {PlayerDirection::down};
	uint16_t texture_index = 0;

	auto operator<=>(const PlayerStateDescription&) const = default;
};

using namespace eclipse::units;

class Player {
public:
	explicit Player(eclipse::ref<eclipse::TextureSheet> texture_sheet,
	                const eclipse::Point2D& position = {.x = pixels(1), .y = pixels(13)},
	                PlayerColor player_color         = PlayerColor::white);
	~Player() = default;
	void on_update(au::QuantityF<au::Seconds> timestep);
	void set_player_position(const eclipse::Point2D& position);
	void set_player_state(PlayerState state);
	void set_player_direction(PlayerDirection direction);
	void set_context(const eclipse::ref<eclipse::Scene>& context);
	void create_player();

	[[nodiscard]] float& get_player_speed() { return player_speed_; }

	[[nodiscard]] uint32_t get_bomb_reach() const { return bomb_reach_; }

	[[nodiscard]] uint32_t get_total_num_of_bombs() const { return total_num_of_bombs_; }

	[[nodiscard]] PlayerState get_player_state() const { return player_state_.state; }

	[[nodiscard]] PlayerStatus get_player_status() const { return player_state_.status; }

	[[nodiscard]] PlayerDirection get_player_direction() const { return player_state_.direction; }

	[[nodiscard]] const eclipse::Point2D& get_player_position() const { return player_position_; }

	void on_bomb_ray_hit() { player_state_.status = PlayerStatus::dead; }

	void on_enemy_collision() { player_state_.status = PlayerStatus::dead; }

	void increase_total_number_of_bombs() { total_num_of_bombs_++; }

	void increase_bomb_reach() { bomb_reach_++; }

	void speed_boost(float boost = 0.5F) { player_speed_ += boost; }

	void invincible() {}

private:
	void update_player_texture();

	au::Quantity<eclipse::units::Pixels, uint32_t> player_character_;
	static constexpr eclipse::Map<PlayerStateDescription, SubTextureIndex, 19> sprite_texture_index_ {
	    {{{{.state = PlayerState::idle, .direction = PlayerDirection::down}, {.x = pixels(0), .y = pixels(19)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::up}, {.x = pixels(0), .y = pixels(10)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::left}, {.x = pixels(0), .y = pixels(13)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::right}, {.x = pixels(0), .y = pixels(16)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::down}, {.x = pixels(0), .y = pixels(18)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::up}, {.x = pixels(0), .y = pixels(9)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::left}, {.x = pixels(0), .y = pixels(12)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::right}, {.x = pixels(0), .y = pixels(15)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::down, .texture_index = 1},
	       {.x = pixels(0), .y = pixels(17)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::up, .texture_index = 1},
	       {.x = pixels(0), .y = pixels(8)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::left, .texture_index = 1},
	       {.x = pixels(0), .y = pixels(11)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::right, .texture_index = 1},
	       {.x = pixels(0), .y = pixels(14)}}}}};

	static constexpr eclipse::Map<DeadPlayerAnimation, SubTextureIndex, 8> player_texture_index_dead_ {
	    {{{DeadPlayerAnimation::first, {.x = pixels(0), .y = pixels(7)}},
	      {DeadPlayerAnimation::second, {.x = pixels(0), .y = pixels(6)}},
	      {DeadPlayerAnimation::third, {.x = pixels(0), .y = pixels(5)}},
	      {DeadPlayerAnimation::forth, {.x = pixels(0), .y = pixels(4)}},
	      {DeadPlayerAnimation::fifth, {.x = pixels(0), .y = pixels(3)}},
	      {DeadPlayerAnimation::sixth, {.x = pixels(0), .y = pixels(2)}},
	      {DeadPlayerAnimation::seventh, {.x = pixels(0), .y = pixels(1)}},
	      {DeadPlayerAnimation::eight, {.x = pixels(0), .y = pixels(0)}}}}};

	static constexpr std::array<DeadPlayerAnimation, 10> DEAD_PLAYER_ANIMATION_SEQUENCE = {
	    DeadPlayerAnimation::first,   DeadPlayerAnimation::second, DeadPlayerAnimation::third, DeadPlayerAnimation::second,
	    DeadPlayerAnimation::third,   DeadPlayerAnimation::forth,  DeadPlayerAnimation::fifth, DeadPlayerAnimation::sixth,
	    DeadPlayerAnimation::seventh, DeadPlayerAnimation::eight};

	eclipse::Point2D player_position_;
	eclipse::Entity player_entity_;
	PlayerStateDescription player_state_ {};
	eclipse::ref<eclipse::Scene> context_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::SubTexture2D> texture_player_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = sprite_texture_index_.at(player_state_).x,
	                                     .tile_index_y  = sprite_texture_index_.at(player_state_).y,
	                                     .tile_width    = pixels(1),
	                                     .tile_height   = pixels(1),
	                                     .offset_x      = pixels(0),
	                                     .offset_y      = pixels(0)});

	float player_speed_                      = 2.5F;
	uint32_t bomb_reach_                     = 1;
	uint32_t total_num_of_bombs_             = 1;
	au::QuantityF<au::Seconds> time_elapsed_ = au::seconds(0.F);
	au::QuantityF<au::Seconds> blink_timer_  = au::seconds(0.F);
	bool texture_blink_ {false};
	uint32_t sprite_animation_sequence_counter_ = 0;
};

}  // namespace bomberman