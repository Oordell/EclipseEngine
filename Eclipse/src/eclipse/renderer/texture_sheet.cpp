#include "ecpch.h"
#include "texture_sheet.h"

namespace eclipse {

TextureSheet::TextureSheet(const TextureSheetProperties& props)
    : texture_(props.texture),
      tile_width_(props.sub_tile_width),
      tile_height_(props.sub_tile_height),
      tile_spacing_x_(props.sub_tile_spacing_x),
      tile_spacing_y_(props.sub_tile_spacing_y) {
	if (tile_width_ > texture_->get_width()) {
		tile_width_ = texture_->get_width();
	}
	if (tile_height_ > texture_->get_height()) {
		tile_height_ = texture_->get_height();
	}

	max_x_index_ = static_cast<int32_t>(texture_->get_width() / tile_width_);
	max_y_index_ = static_cast<int32_t>(texture_->get_height() / tile_height_);
}

std::string TextureSheet::get_debug_string() const {
	std::stringstream ss;
	ss << "Tex Sheet: \n";
	ss << " - Texture width:             " << get_texture()->get_width() << "\n";
	ss << " - Texture height:            " << get_texture()->get_height() << "\n";
	ss << " - Tile width in pixels:      " << get_tile_width_in_pixels() << "\n";
	ss << " - Tile height in pixels:     " << get_tile_height_in_pixels() << "\n";
	ss << " - Tile width normalized:     " << get_tile_width_normalized() << "\n";
	ss << " - Tile height normalized:    " << get_tile_height_normalized() << "\n";
	ss << " - Tile spacing x in pixels:  " << get_tile_spacing_x_in_pixels() << "\n";
	ss << " - Tile spacing y in pixels:  " << get_tile_spacing_y_in_pixels() << "\n";
	ss << " - Tile spacing x normalized: " << get_tile_spacing_x_normalized() << "\n";
	ss << " - Tile spacing y normalized: " << get_tile_spacing_y_normalized() << "\n";
	return ss.str();
}

}  // namespace eclipse