#pragma once

#include "eclipse/common_types/units/pixel.h"
#include "eclipse/core/core.h"

namespace eclipse {

struct ECLIPSE_API MouseScrollOffset {
	au::QuantityF<units::Pixels> x {units::pixels(0.0F)};
	au::QuantityF<units::Pixels> y {units::pixels(0.0F)};
};

}  // namespace eclipse