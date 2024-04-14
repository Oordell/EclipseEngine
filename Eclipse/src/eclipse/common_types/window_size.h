#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/units/pixel.h"

namespace eclipse {

struct ECLIPSE_API WindowSize {
	au::Quantity<units::Pixels, uint32_t> width {units::pixels(0)};
	au::Quantity<units::Pixels, uint32_t> height {units::pixels(0)};

	[[nodiscard]] inline constexpr float get_aspect_ratio() const {
		return width.in<float>(units::pixels) / height.in<float>(units::pixels);
	}
};

}  // namespace eclipse