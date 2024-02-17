#pragma once

#include "eclipse/core/core.h"

#include "eclipse/common_types/units/pixels_per_seconds.h"

namespace eclipse {

struct ECLIPSE_API ScreenVelocity2D {
	au::Quantity<PixelsPerSeconds, float> vel_x = pixels_per_seconds(0.0F);
	au::Quantity<PixelsPerSeconds, float> vel_y = pixels_per_seconds(0.0F);
};

}  // namespace eclipse