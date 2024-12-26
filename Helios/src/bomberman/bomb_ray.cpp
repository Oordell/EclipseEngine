#include <ecpch.h>
#include "bomb_ray.hpp"

namespace bomberman {
void BombRay::destroy() { context_->destroy_entity(entity_); }

void BombRay::on_update(au::QuantityF<au::Seconds> timestep) {
	blink_timer_ += timestep;
	static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.15F);
	if (blink_timer_ > blink_timer_toggle) {
		blink_timer_ = au::seconds(0.F);

		sprite_animation_sequence_counter_ += 1;
		if (sprite_animation_sequence_counter_ >= bomb_ray_animation_sequence_.size()) {
			expired_                           = BombRayExpired::yes;
			sprite_animation_sequence_counter_ = 0;
		}

		bomb_ray_state_.size = bomb_ray_animation_sequence_.at(sprite_animation_sequence_counter_);

		update_texture();
	}
}

void BombRay::create_entity(const std::string& entity_name /* = "Bomb Ray" */) {
	texture_bomb_ray_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = bomb_ray_sprites_.at(bomb_ray_state_).x,
	                                     .tile_index_y  = bomb_ray_sprites_.at(bomb_ray_state_).y,
	                                     .tile_width    = pixels(1),
	                                     .tile_height   = pixels(1)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_bomb_ray_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_BOMB_RAY;
}

void BombRay::update_texture() {
	SubTextureIndex sub_texture_index = bomb_ray_sprites_.at(bomb_ray_state_);
	texture_bomb_ray_->set_index_x(sub_texture_index.x);
	texture_bomb_ray_->set_index_y(sub_texture_index.y);
}
}  // namespace bomberman