#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

enum class PlayerState { idle, running };
enum class PlayerStatus { alive, dead, game_over };
enum class PlayerDirection { up, down, left, right };

struct PlayerStateDescription {
	PlayerStatus status {PlayerStatus::alive};
	PlayerState state {PlayerState::idle};
	PlayerDirection direction {PlayerDirection::down};
	uint16_t texture_index = 0;

	auto operator<=>(const PlayerStateDescription&) const = default;
};

enum class DeadPlayerAnimation { first, second, third, forth, fifth, sixth, seventh, eight };

static constexpr std::array<DeadPlayerAnimation, 10> DEAD_PLAYER_ANIMATION_SEQUENCE = {
    DeadPlayerAnimation::first,   DeadPlayerAnimation::second, DeadPlayerAnimation::third, DeadPlayerAnimation::second,
    DeadPlayerAnimation::third,   DeadPlayerAnimation::forth,  DeadPlayerAnimation::fifth, DeadPlayerAnimation::sixth,
    DeadPlayerAnimation::seventh, DeadPlayerAnimation::eight};

using namespace eclipse::units;

class Player {
public:
	explicit Player(eclipse::ref<eclipse::TextureSheet> texture_sheet,
	                const eclipse::Point2D& position = {.x = pixels(1), .y = pixels(13)});
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

	void on_bomb_ray_hit() {
		EC_DEBUG("BOMB COLLISION!");
		player_state_.status = PlayerStatus::dead;
	}

	void on_enemy_collision() {
		EC_DEBUG("ENEMY COLLISION!");
		player_state_.status = PlayerStatus::dead;
	}

	void increase_total_number_of_bombs() { total_num_of_bombs_++; }

	void increase_bomb_reach() { bomb_reach_++; }

	void speed_boost(float boost = 0.5F) { player_speed_ += boost; }

	void invincible() {}

private:
	void update_player_texture();

	static constexpr eclipse::Map<PlayerStateDescription, SubTextureIndex, 19> sprite_texture_index_ {
	    {{{{.state = PlayerState::idle, .direction = PlayerDirection::down}, {.x = pixels(0), .y = pixels(5)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::up}, {.x = pixels(0), .y = pixels(4)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::left}, {.x = pixels(0), .y = pixels(6)}},
	      {{.state = PlayerState::idle, .direction = PlayerDirection::right}, {.x = pixels(0), .y = pixels(6)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::down}, {.x = pixels(1), .y = pixels(5)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::up}, {.x = pixels(1), .y = pixels(4)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::left}, {.x = pixels(1), .y = pixels(6)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::right}, {.x = pixels(1), .y = pixels(6)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::down, .texture_index = 1},
	       {.x = pixels(2), .y = pixels(5)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::up, .texture_index = 1},
	       {.x = pixels(2), .y = pixels(4)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::left, .texture_index = 1},
	       {.x = pixels(2), .y = pixels(6)}},
	      {{.state = PlayerState::running, .direction = PlayerDirection::right, .texture_index = 1},
	       {.x = pixels(2), .y = pixels(6)}}}}};

	static constexpr eclipse::Map<DeadPlayerAnimation, SubTextureIndex, 8> player_texture_index_dead_ {
	    {{{DeadPlayerAnimation::first, {.x = pixels(0), .y = pixels(5)}},
	      {DeadPlayerAnimation::second, {.x = pixels(0), .y = pixels(3)}},
	      {DeadPlayerAnimation::third, {.x = pixels(1), .y = pixels(3)}},
	      {DeadPlayerAnimation::forth, {.x = pixels(2), .y = pixels(3)}},
	      {DeadPlayerAnimation::fifth, {.x = pixels(3), .y = pixels(3)}},
	      {DeadPlayerAnimation::sixth, {.x = pixels(4), .y = pixels(3)}},
	      {DeadPlayerAnimation::seventh, {.x = pixels(5), .y = pixels(3)}},
	      {DeadPlayerAnimation::eight, {.x = pixels(6), .y = pixels(3)}}}}};

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
	                                     .offset_y      = pixels(7)});

	float player_speed_                      = 2.5F;
	uint32_t bomb_reach_                     = 1;
	uint32_t total_num_of_bombs_             = 1;
	au::QuantityF<au::Seconds> time_elapsed_ = au::seconds(0.F);
	au::QuantityF<au::Seconds> blink_timer_  = au::seconds(0.F);
	bool texture_blink_ {false};
	uint32_t sprite_animation_sequence_counter_ = 0;
};

}  // namespace bomberman