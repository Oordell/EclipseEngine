#include "ecpch.h"
#include "sub_texture_2d.h"

namespace eclipse {

SubTexture2D::SubTexture2D(const SubTexture2DProperties& props)
    : texture_sheet_(props.texture_sheet),
      x_min_(props.tile_index_x),
      y_min_(props.tile_index_y),
      tile_width_(props.tile_width),
      tile_height_(props.tile_height) {
	float x0 = x_min_.in<float>(units::pixels) * texture_sheet_->get_tile_width_normalized() +
	           x_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_x_normalized();
	float y0 = y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_height_normalized() +
	           y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_y_normalized();

	float x1 = (x_min_ + tile_width_).in<float>(units::pixels) * texture_sheet_->get_tile_width_normalized() +
	           x_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_x_normalized();
	float y1 = y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_height_normalized() +
	           y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_y_normalized();

	float x2 = (x_min_ + tile_width_).in<float>(units::pixels) * texture_sheet_->get_tile_width_normalized() +
	           x_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_x_normalized();
	float y2 = (y_min_ + tile_height_).in<float>(units::pixels) * texture_sheet_->get_tile_height_normalized() +
	           y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_y_normalized();

	float x3 = (x_min_.in<float>(units::pixels) + 0) * texture_sheet_->get_tile_width_normalized() +
	           x_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_x_normalized();
	float y3 = (y_min_ + tile_height_).in<float>(units::pixels) * texture_sheet_->get_tile_height_normalized() +
	           y_min_.in<float>(units::pixels) * texture_sheet_->get_tile_spacing_y_normalized();

	texture_coords_[0] = {x0, y0};
	texture_coords_[1] = {x1, y1};
	texture_coords_[2] = {x2, y2};
	texture_coords_[3] = {x3, y3};
};

}  // namespace eclipse