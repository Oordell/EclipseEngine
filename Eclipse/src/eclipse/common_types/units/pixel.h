#pragma once

#include "eclipse/common_types/dimensions/pixel.h"

namespace eclipse::units {

struct Pixels : au::UnitImpl<au::Dimension<dim::PixelBaseDim>> {
	static constexpr inline const char label[] = "px";
};

constexpr auto pixel  = au::SingularNameFor<Pixels> {};
constexpr auto pixels = au::QuantityMaker<Pixels> {};

}  // namespace eclipse::units