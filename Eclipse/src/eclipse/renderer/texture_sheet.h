#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/texture.h"

namespace eclipse {

struct TextureSheetProperties {
	ref<Texture2D> texture;
	int32_t sub_tile_width {1};
	int32_t sub_tile_height {1};
	int32_t sub_tile_spacing_x {0};
	int32_t sub_tile_spacing_y {0};
};

class TextureSheet {
public:
	TextureSheet(const TextureSheetProperties& props);

	~TextureSheet() = default;

	const ref<Texture2D> get_texture() const { return texture_; }

	const int32_t get_tile_width_in_pixels() const { return tile_width_; }

	const int32_t get_tile_height_in_pixels() const { return tile_height_; }

	const int32_t get_tile_spacing_x_in_pixels() const { return tile_spacing_x_; }

	const int32_t get_tile_spacing_y_in_pixels() const { return tile_spacing_y_; }

	const float get_tile_spacing_x_normalized() const {
		return static_cast<float>(tile_spacing_x_) / static_cast<float>(texture_->get_width());
	}

	const float get_tile_spacing_y_normalized() const {
		return static_cast<float>(tile_spacing_y_) / static_cast<float>(texture_->get_height());
	}

	const float get_tile_width_normalized() const {
		return static_cast<float>(tile_width_) / static_cast<float>(texture_->get_width());
	}

	const float get_tile_height_normalized() const {
		return static_cast<float>(tile_height_) / static_cast<float>(texture_->get_height());
	}

	std::string get_debug_string() const;

private:
	const ref<Texture2D> texture_;
	int32_t tile_width_;
	int32_t tile_height_;
	int32_t tile_spacing_x_;
	int32_t tile_spacing_y_;
	int32_t max_x_index_ {0};
	int32_t max_y_index_ {0};
};

}  // namespace eclipse