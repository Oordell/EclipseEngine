#pragma once

#include "eclipse/common_types/units/pixel.h"
#include "eclipse/core/core.h"

namespace eclipse {

template <typename Rep>
struct ECLIPSE_API Point2D_ {
	au::Quantity<units::Pixels, Rep> x {units::pixels(static_cast<Rep>(0.0))};
	au::Quantity<units::Pixels, Rep> y {units::pixels(static_cast<Rep>(0.0))};

	auto operator<=>(const Point2D_&) const = default;
};

using Point2D  = Point2D_<float>;
using Point2DI = Point2D_<uint32_t>;

}  // namespace eclipse