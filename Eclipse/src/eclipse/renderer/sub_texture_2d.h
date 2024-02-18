#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/texture_sheet.h"
#include "eclipse/common_types/units/pixel.h"

#include <glm/glm.hpp>

namespace eclipse {

struct SubTexture2DProperties {
	ref<TextureSheet> texture_sheet;
	au::Quantity<units::Pixels, uint32_t> tile_index_x;
	au::Quantity<units::Pixels, uint32_t> tile_index_y;
	au::Quantity<units::Pixels, uint32_t> tile_width {units::pixels(1)};
	au::Quantity<units::Pixels, uint32_t> tile_height {units::pixels(1)};
};

class SubTexture2D {
public:
	SubTexture2D(const SubTexture2DProperties& props);
	~SubTexture2D() = default;

	const glm::vec2* get_texture_coords() const { return texture_coords_; }

	ref<TextureSheet> get_texture_sheet() const { return texture_sheet_; }

	ref<Texture2D> get_texture() const { return texture_sheet_->get_texture(); }

	au::Quantity<units::Pixels, uint32_t> get_width() const { return tile_width_; }

	au::Quantity<units::Pixels, uint32_t> get_height() const { return tile_height_; }

private:
	ref<TextureSheet> texture_sheet_;
	au::Quantity<units::Pixels, uint32_t> x_min_;
	au::Quantity<units::Pixels, uint32_t> y_min_;
	au::Quantity<units::Pixels, uint32_t> tile_width_;
	au::Quantity<units::Pixels, uint32_t> tile_height_;
	glm::vec2 texture_coords_[4] {};
};

}  // namespace eclipse