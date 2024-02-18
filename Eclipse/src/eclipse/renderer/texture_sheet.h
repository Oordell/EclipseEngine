#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/texture.h"

namespace eclipse {

struct TextureSheetProperties {
	ref<Texture2D> texture;
	au::Quantity<units::Pixels, uint32_t> sub_tile_width {units::pixels(1)};
	au::Quantity<units::Pixels, uint32_t> sub_tile_height {units::pixels(1)};
	au::Quantity<units::Pixels, uint32_t> sub_tile_spacing_x {units::pixels(0)};
	au::Quantity<units::Pixels, uint32_t> sub_tile_spacing_y {units::pixels(0)};
};

class TextureSheet {
public:
	TextureSheet(const TextureSheetProperties& props);

	~TextureSheet() = default;

	const ref<Texture2D> get_texture() const { return texture_; }

	const au::Quantity<units::Pixels, uint32_t> get_tile_width_in_pixels() const { return tile_width_; }

	const au::Quantity<units::Pixels, uint32_t> get_tile_height_in_pixels() const { return tile_height_; }

	const au::Quantity<units::Pixels, uint32_t> get_tile_spacing_x_in_pixels() const { return tile_spacing_x_; }

	const au::Quantity<units::Pixels, uint32_t> get_tile_spacing_y_in_pixels() const { return tile_spacing_y_; }

	[[nodiscard]] inline constexpr float get_tile_spacing_x_normalized() const {
		return tile_spacing_x_.in<float>(units::pixels) / texture_->get_width().in<float>(units::pixels);
	}

	[[nodiscard]] inline constexpr float get_tile_spacing_y_normalized() const {
		return tile_spacing_y_.in<float>(units::pixels) / texture_->get_height().in<float>(units::pixels);
	}

	[[nodiscard]] inline constexpr float get_tile_width_normalized() const {
		return tile_width_.in<float>(units::pixels) / texture_->get_width().in<float>(units::pixels);
	}

	[[nodiscard]] inline constexpr float get_tile_height_normalized() const {
		return tile_height_.in<float>(units::pixels) / texture_->get_height().in<float>(units::pixels);
	}

	std::string get_debug_string() const;

private:
	const ref<Texture2D> texture_;
	au::Quantity<units::Pixels, uint32_t> tile_width_;
	au::Quantity<units::Pixels, uint32_t> tile_height_;
	au::Quantity<units::Pixels, uint32_t> tile_spacing_x_;
	au::Quantity<units::Pixels, uint32_t> tile_spacing_y_;
	au::Quantity<units::Pixels, uint32_t> max_x_index_ {units::pixels(0)};
	au::Quantity<units::Pixels, uint32_t> max_y_index_ {units::pixels(0)};
};

}  // namespace eclipse