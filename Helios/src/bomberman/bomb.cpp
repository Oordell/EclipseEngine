#include "ecpch.h"
#include "bomb.hpp"

namespace bomberman {
Bomb::Bomb(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
           const eclipse::ref<eclipse::Scene>& context, const OnExplosionCallback& on_explosion_callback,
           uint32_t reach /* = 1*/)
    : texture_sheet_(texture_sheet), position_(position), callback_(on_explosion_callback), context_(context) {
	create_entity();
	static constexpr uint32_t MAX_REACH = 10;
	static constexpr uint32_t MIN_REACH = 1;
	bomb_reach_                         = std::clamp(reach, MIN_REACH, MAX_REACH);
}

void Bomb::on_bomb_ray_hit() { bomb_explosion(); }

void Bomb::on_update(au::QuantityF<au::Seconds> timestep) {
	blink_timer_ += timestep;
	lifetime_timer_ += timestep;

	if (bomb_state_ == BombState::placed && lifetime_timer_ > bomb_detonation_counter_) {
		bomb_explosion();
	}
	static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.15F);
	if (blink_timer_ > blink_timer_toggle) {
		blink_timer_ = au::seconds(0.F);

		sprite_animation_sequence_counter_ += 1;
		if (bomb_state_ == BombState::placed) {
			if (sprite_animation_sequence_counter_ >= bomb_placed_animation_sequence_.size()) {
				sprite_animation_sequence_counter_ = 0;
			}
		} else if (bomb_state_ == BombState::exploded) {
			if (sprite_animation_sequence_counter_ >= bomb_exploded_animation_sequence_.size()) {
				sprite_animation_sequence_counter_ = 0;
				bomb_state_                        = BombState::expired;
			}
		}

		update_texture();
	}
}

void Bomb::create_entity(const std::string& entity_name /* = "Bomb"*/) {
	texture_bomb_ = eclipse::make_ref<eclipse::SubTexture2D>(eclipse::SubTexture2DProperties {
	    .texture_sheet = texture_sheet_,
	    .tile_index_x  = bomb_sprite_placed_.at(bomb_placed_animation_sequence_.at(sprite_animation_sequence_counter_)).x,
	    .tile_index_y  = bomb_sprite_placed_.at(bomb_placed_animation_sequence_.at(sprite_animation_sequence_counter_)).y,
	    .tile_width    = pixels(1),
	    .tile_height   = pixels(1),
	    .offset_x      = pixels(0),
	    .offset_y      = pixels(0)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_bomb_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_BOMB;
}

void Bomb::bomb_explosion() {
	bomb_state_                        = BombState::exploded;
	blink_timer_                       = au::seconds(0.F);
	sprite_animation_sequence_counter_ = 0;
	callback_(position_, bomb_reach_);
}

void Bomb::update_texture() {
	SubTextureIndex sub_texture_index;
	if (bomb_state_ == BombState::placed) {
		sub_texture_index = bomb_sprite_placed_.at(bomb_placed_animation_sequence_.at(sprite_animation_sequence_counter_));
	} else if (bomb_state_ == BombState::exploded) {
		sub_texture_index =
		    bomb_sprite_exploded_.at(bomb_exploded_animation_sequence_.at(sprite_animation_sequence_counter_));
	}

	texture_bomb_->set_index_x(sub_texture_index.x);
	texture_bomb_->set_index_y(sub_texture_index.y);
}

}  // namespace bomberman