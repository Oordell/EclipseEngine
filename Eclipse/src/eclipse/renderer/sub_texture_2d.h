#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/texture_sheet.h"

#include <glm/glm.hpp>

namespace eclipse {

struct SubTexture2DProperties {
	ref<TextureSheet> texture_sheet;
	int32_t tile_index_x;
	int32_t tile_index_y;
	float tile_width {1.0F};
	float tile_height {1.0F};
};

class SubTexture2D {
public:
	SubTexture2D(const SubTexture2DProperties& props);
	~SubTexture2D() = default;

	const glm::vec2* get_texture_coords() const { return texture_coords_; }

	ref<TextureSheet> get_texture_sheet() const { return texture_sheet_; }

	ref<Texture2D> get_texture() const { return texture_sheet_->get_texture(); }

	float get_width() const { return tile_width_; }

	float get_height() const { return tile_height_; }

private:
	ref<TextureSheet> texture_sheet_;
	int32_t x_min_;
	int32_t y_min_;
	float tile_width_;
	float tile_height_;
	glm::vec2 texture_coords_[4] {};
};

}  // namespace eclipse