#include "ecpch.h"
#include "soft_wall.hpp"

namespace bomberman {

SoftWall::SoftWall(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
                   const eclipse::ref<eclipse::Scene>& context)
    : texture_sheet_(texture_sheet), position_(position), context_(context) {
	create_entity();
}

void SoftWall::on_update(au::QuantityF<au::Seconds> timestep) {
	if (wall_state_ == WallState::idle) {
		return;
	} else if (wall_state_ == WallState::exploding) {
		blink_timer_ += timestep;
		static constexpr au::QuantityF<au::Seconds> blink_timer_toggle = au::seconds(0.15F);
		if (blink_timer_ > blink_timer_toggle) {
			blink_timer_ = au::seconds(0.F);

			current_wall_texture_index_ += 1;
			if (current_wall_texture_index_ >= wall_destruction_animation_sequence_.size()) {
				wall_state_                 = WallState::expired;
				current_wall_texture_index_ = 0;
			}

			update_texture();
		}
	}
}

void SoftWall::on_bomb_ray_hit() {
	wall_state_                 = WallState::exploding;
	current_wall_texture_index_ = 0;
}

void SoftWall::update_texture() {
	const auto& sub_texture_index =
	    wall_texture_index_.at(wall_destruction_animation_sequence_.at(current_wall_texture_index_));
	texture_wall_->set_index_x(sub_texture_index.x);
	texture_wall_->set_index_y(sub_texture_index.y);
}

void SoftWall::create_entity(const std::string& entity_name /*= "Soft wall"*/) {
	texture_wall_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = texture_sheet_coordinates_.x,
	                                     .tile_index_y  = texture_sheet_coordinates_.y,
	                                     .tile_width    = pixels(1),
	                                     .tile_height   = pixels(1),
	                                     .offset_x      = pixels(0),
	                                     .offset_y      = pixels(0)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_wall_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_SOFT_WALS;
}

}  // namespace bomberman