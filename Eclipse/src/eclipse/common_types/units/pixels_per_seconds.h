#pragma once

#include <au_noio.hh>
#include "pixel.h"

namespace eclipse {

struct PixelsPerSeconds : decltype(Pixels {} / au::Seconds {}) {
	static constexpr inline const char label[] = "px/s";
};

constexpr auto pixels_per_second     = au::SingularNameFor<PixelsPerSeconds> {};
constexpr auto pixels_per_seconds    = au::QuantityMaker<PixelsPerSeconds> {};
constexpr auto pixels_per_seconds_pt = au::QuantityPointMaker<PixelsPerSeconds> {};

namespace symbols {
constexpr auto pxprs = au::SymbolFor<PixelsPerSeconds> {};
}

}  // namespace eclipse