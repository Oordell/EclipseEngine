#pragma once

#include "eclipse/common_types/dimensions/dim_pixel.h"

namespace eclipse {

struct Pixels : au::UnitImpl<au::Dimension<PixelBaseDim>> {
	static constexpr inline const char label[] = "px";
};

constexpr auto pixel  = au::SingularNameFor<Pixels> {};
constexpr auto pixels = au::QuantityMaker<Pixels> {};

}  // namespace eclipse