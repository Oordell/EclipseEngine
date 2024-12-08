#include "ecpch.h"
#include "player.hpp"

namespace bomberman {
Player::Player(eclipse::ref<eclipse::TextureSheet> texture_sheet,
               const eclipse::Point2D& position /* = {.x = pixels(1), .y = pixels(13)}*/)
    : texture_sheet_(texture_sheet), player_position_(position) {}

void Player::on_update(au::QuantityF<au::Seconds> timestep) {
	time_elapsed_ += timestep;
	blink_timer_ += timestep;

	static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.15F);
	if (blink_timer_ > blink_timer_toggle) {
		blink_timer_   = au::seconds(0.F);
		texture_blink_ = !texture_blink_;

		if (player_state_.status == PlayerStatus::alive) {
			if (player_state_.state == PlayerState::idle) {
				player_state_.texture_index = 0;
			} else if (player_state_.state == PlayerState::running) {
				player_state_.texture_index = texture_blink_ ? 1 : 0;
			}
			update_player_texture();
		} else if (player_state_.status == PlayerStatus::dead) {
			sprite_animation_sequence_counter_++;
			if (sprite_animation_sequence_counter_ >= DEAD_PLAYER_ANIMATION_SEQUENCE.size()) {
				sprite_animation_sequence_counter_ = 0;
				player_state_.status               = PlayerStatus::game_over;
			}
			player_state_.texture_index = 0;
			update_player_texture();
		} else if (player_state_.status == PlayerStatus::game_over) {
			//	update_player_texture(sprite_texture_index_.at(player_state_));
		}
	}
}

void Player::set_player_position(const eclipse::Point2D& position) {
	player_position_                                                            = position;
	player_entity_.get_component<eclipse::component::Transform>().translation.x = player_position_.x.in<float>(pixels);
	player_entity_.get_component<eclipse::component::Transform>().translation.y = player_position_.y.in<float>(pixels);
}

void Player::set_player_state(PlayerState state) {
	player_state_.state = state;
	if (player_state_.state == PlayerState::idle) {
		player_state_.texture_index = 0;
	}
	update_player_texture();
}

void Player::set_player_direction(PlayerDirection direction) {
	player_state_.direction = direction;
	player_entity_.get_component<eclipse::component::Transform>().rotation.y =
	    direction == PlayerDirection::left ? au::degrees(180.F).in(au::radians) : 0.F;

	update_player_texture();
}

void Player::set_context(const eclipse::ref<eclipse::Scene>& context) { context_ = context; }

void Player::create_player() {
	player_state_ = {};

	player_entity_ = context_->create_entity("player");
	player_entity_.add_component<eclipse::component::SubTexture>(texture_player_);

	player_entity_.get_component<eclipse::component::Transform>().translation.x = player_position_.x.in<float>(pixels);
	player_entity_.get_component<eclipse::component::Transform>().translation.y = player_position_.y.in<float>(pixels);
	player_entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_PLAYER;
}

void Player::update_player_texture() {
	SubTextureIndex index;

	if (player_state_.status == PlayerStatus::alive) {
		index = sprite_texture_index_.at(player_state_);
	} else if (player_state_.status == PlayerStatus::dead) {
		player_state_.texture_index = 0;
		index = player_texture_index_dead_.at(DEAD_PLAYER_ANIMATION_SEQUENCE.at(sprite_animation_sequence_counter_));
	} else if (player_state_.status == PlayerStatus::game_over) {
		player_state_.texture_index = 0;
		index = player_texture_index_dead_.at(DEAD_PLAYER_ANIMATION_SEQUENCE.at(sprite_animation_sequence_counter_));
	}

	texture_player_->set_index_x(index.x);
	texture_player_->set_index_y(index.y);
}
}  // namespace bomberman