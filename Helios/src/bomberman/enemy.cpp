#include "ecpch.h"
#include "enemy.hpp"

namespace bomberman {

Enemy::Enemy(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
             const eclipse::ref<eclipse::Scene>& context)
    : texture_sheet_(texture_sheet), position_(position), context_(context) {}

void Enemy::on_update(au::QuantityF<au::Seconds> timestep) {
	blink_timer_ += timestep;
	static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.25F);
	if (blink_timer_ > blink_timer_toggle) {
		blink_timer_ = au::seconds(0.F);
		sprite_animation_sequence_counter_ += 1;

		if (enemy_state_ == EnemyState::alive) {
			if (sprite_animation_sequence_counter_ >= SPRITE_ALIVE_ANIMATION_SEQUENCE.size()) {
				sprite_animation_sequence_counter_ = 0;
				sprite_alive_.flip_direction();
			}
			sprite_alive_.animation = SPRITE_ALIVE_ANIMATION_SEQUENCE.at(sprite_animation_sequence_counter_);
		} else if (enemy_state_ == EnemyState::dead) {
			if (sprite_animation_sequence_counter_ >= SPRITE_DEAD_ANIMATION_SEQUENCE.size()) {
				sprite_animation_sequence_counter_ = 0;
				enemy_state_                       = EnemyState::expired;
			}
			sprite_dead_ = SPRITE_DEAD_ANIMATION_SEQUENCE.at(sprite_animation_sequence_counter_);
		}

		update_texture();
	}
}

void Enemy::destroy() {
	entity_.remove_component<eclipse::component::SubTexture>();
	context_->destroy_entity(entity_);
}

void Enemy::on_bomb_ray_hit() {
	if (enemy_state_ != EnemyState::alive) {
		// Enemy has already been hit
		return;
	}
	enemy_state_                       = EnemyState::dead;
	sprite_animation_sequence_counter_ = 0;
	sprite_dead_                       = SPRITE_DEAD_ANIMATION_SEQUENCE.at(sprite_animation_sequence_counter_);
	update_texture();
}

void Enemy::create_entity(const std::string& entity_name, const SubTextureIndex& index) {
	texture_enemy_ =
	    eclipse::make_ref<eclipse::SubTexture2D>(eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                                                              .tile_index_x  = index.x,
	                                                                              .tile_index_y  = index.y,
	                                                                              .tile_width    = pixels(1),
	                                                                              .tile_height   = pixels(1),
	                                                                              .offset_x      = pixels(0),
	                                                                              .offset_y      = pixels(15)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_enemy_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_ENEMIES;
}

}  // namespace bomberman