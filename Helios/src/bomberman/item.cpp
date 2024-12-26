#include "ecpch.h"
#include "item.hpp"

namespace bomberman {
Item::Item(eclipse::ref<eclipse::TextureSheet> texture_sheet, const eclipse::Point2D& position,
           const eclipse::ref<eclipse::Scene>& context, const SubTextureIndex& texture_sheet_coordinates)
    : texture_sheet_(texture_sheet),
      position_(position),
      context_(context),
      texture_sheet_coordinates_(texture_sheet_coordinates) {
	texture_item_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = texture_sheet_,
	                                     .tile_index_x  = texture_sheet_coordinates_.x,
	                                     .tile_index_y  = texture_sheet_coordinates_.y,
	                                     .tile_width    = eclipse::units::pixels(1),
	                                     .tile_height   = eclipse::units::pixels(1)});
	create_entity();
}

void Item::create_entity() {
	entity_ = context_->create_entity("ability");
	entity_.add_component<eclipse::component::SubTexture>(texture_item_);

	entity_.get_component<eclipse::component::Transform>().translation.x = position_.x.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.y = position_.y.in(eclipse::units::pixels);
	entity_.get_component<eclipse::component::Transform>().translation.z = details::Z_ITEMS;

	entity_.get_component<eclipse::component::Transform>().scale.x = 0.95F;
	entity_.get_component<eclipse::component::Transform>().scale.y = 0.95F;
}

}  // namespace bomberman