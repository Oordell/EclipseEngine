#include "ecpch.h"
#include "frame.hpp"

namespace bomberman {
Frame::Frame(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
             const eclipse::ref<eclipse::Scene>& context, const SubTextureIndex& texture_sheet_coordinates)
    : texture_sheet_(texture_sheet),
      position_(position),
      context_(context),
      texture_sheet_coordinates_(texture_sheet_coordinates) {
	create_entity();
}

void Frame::create_entity(const std::string& entity_name /*= "Frame"*/) {
	texture_frame_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = texture_sheet_coordinates_.x,
	                                     .tile_index_y  = texture_sheet_coordinates_.y,
	                                     .tile_width    = pixels(1),
	                                     .tile_height   = pixels(1)});

	entity_ = context_->create_entity(entity_name);
	entity_.add_component<eclipse::component::SubTexture>(texture_frame_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_SOLID_WALLS;
}

}  // namespace bomberman