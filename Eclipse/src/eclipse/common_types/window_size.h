#pragma once

#include "eclipse/core/core.h"

namespace eclipse {

struct ECLIPSE_API WindowSize {
	uint32_t width {0};
	uint32_t height {0};

	float get_aspect_ratio() const { return static_cast<float>(width) / static_cast<float>(height); }
};

}  // namespace eclipse