#include "ecpch.h"
#include "enemy.hpp"
#include "movement_random.hpp"
#include "movement_none.hpp"

namespace bomberman {

Enemy::Enemy(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
             const eclipse::ref<eclipse::Scene>& context, PlayingBoard& playing_board, const EnemyMetaData& enemy_data)
    : texture_sheet_(texture_sheet), position_(position), context_(context), enemy_data_(enemy_data) {
	switch (enemy_data_.movement_type) {
		case EnemyMovementType::state_based:
		case EnemyMovementType::none: {
			movement_ = eclipse::make_scope<MovementNone>();
			break;
		}
		case EnemyMovementType::random: {
			movement_ = eclipse::make_scope<MovementRandom>(
			    EC_BIND_EVENT_FN(Enemy::get_enemy_meta_data), EC_BIND_EVENT_FN(Enemy::get_position),
			    EC_BIND_EVENT_FN(Enemy::set_enemy_direction), EC_BIND_EVENT_FN(Enemy::set_enemy_position), playing_board,
			    MovementSettings {.ignore_soft_walls = enemy_data_.ignore_soft_walls});
			break;
		}
	}
}

void Enemy::on_update(au::QuantityF<au::Seconds> timestep) {
	if (enemy_state_ == EnemyState::alive) {
		movement_->on_update(timestep);
	}
	blink_timer_ += timestep;
	static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.15F);
	if (blink_timer_ > blink_timer_toggle) {
		blink_timer_ = au::seconds(0.F);
		sprite_animation_sequence_counter_ += 1;

		if (enemy_state_ == EnemyState::alive) {
			if (sprite_animation_sequence_counter_ >= SPRITE_ALIVE_ANIMATION_SEQUENCE.size()) {
				sprite_animation_sequence_counter_ = 0;
				//	sprite_alive_.flip_direction();
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

void Enemy::create_entity(const std::string& entity_name) {
	const auto& index = enemy_texture_index_alive_.at(sprite_alive_);
	texture_enemy_    = eclipse::make_ref<eclipse::SubTexture2D>(
     eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                        .tile_index_x  = index.x,
	                                        .tile_index_y  = index.y + pixels(static_cast<uint32_t>(enemy_data_.enemy_type)),
	                                        .tile_width    = pixels(1),
	                                        .tile_height   = pixels(1)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_enemy_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_ENEMIES;
}

void Enemy::update_texture() {
	if (enemy_state_ == EnemyState::alive) {
		const auto& sub_texture_index = enemy_texture_index_alive_.at(sprite_alive_);
		texture_enemy_->set_index_x(sub_texture_index.x);
		texture_enemy_->set_index_y(sub_texture_index.y + pixels(static_cast<uint32_t>(enemy_data_.enemy_type)));
	} else if (enemy_state_ == EnemyState::dead) {
		const auto& sub_texture_index = enemy_texture_index_dead_.at(sprite_dead_);
		texture_enemy_->set_index_x(sub_texture_index.x);
		texture_enemy_->set_index_y(sub_texture_index.y + pixels(static_cast<uint32_t>(enemy_data_.enemy_type)));
	}
}

}  // namespace bomberman