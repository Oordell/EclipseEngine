#pragma once

#include "Eclipse.h"
#include "common_types.hpp"

namespace bomberman {

using eclipse::units::pixels;

class SolidWall : public EntityInterface {
public:
	explicit SolidWall(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
	                   const eclipse::ref<eclipse::Scene>& context)
	    : texture_sheet_(texture_sheet), position_(position), context_(context) {
		create_entity();
	}

	void on_update(au::QuantityF<au::Seconds> timestep) override {}

	void destroy() override {}

	void on_bomb_ray_hit() override {}

	CellContent on_player_interaction() override { return CellContent::solid_wall; }

	[[nodiscard]] bool has_expired() const override { return false; }

	[[nodiscard]] virtual const eclipse::Point2D& get_position() const override { return position_; }

protected:
	void update_texture() override {};

private:
	void create_entity(const std::string& entity_name = "Solid wall") {
		texture_wall_ =
		    eclipse::make_ref<eclipse::SubTexture2D>(eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
		                                                                              .tile_index_x  = pixels(3),
		                                                                              .tile_index_y  = pixels(17),
		                                                                              .tile_width    = pixels(1),
		                                                                              .tile_height   = pixels(1),
		                                                                              .offset_x      = pixels(0),
		                                                                              .offset_y      = pixels(0)});

		entity_ = context_->create_entity(entity_name);
		entity_.add_component<eclipse::component::SubTexture>(texture_wall_);

		entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
		entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
		entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_SOLID_WALLS;
	}

	eclipse::Point2D position_;
	eclipse::ref<eclipse::TextureSheet> texture_sheet_;
	eclipse::ref<eclipse::Scene> context_;
	eclipse::Entity entity_;
	eclipse::ref<eclipse::SubTexture2D> texture_wall_;
};

}  // namespace bomberman