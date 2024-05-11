#pragma once

#include <au.hh>

namespace eclipse::units {

struct Newtons : decltype(au::Kilo<au::Grams> {} * au::Meters {} / au::squared(au::Seconds {})) {
	static constexpr inline const char label[] = "kg * m / s^2";
};

constexpr auto newton     = au::SingularNameFor<Newtons> {};
constexpr auto newtons    = au::QuantityMaker<Newtons> {};
constexpr auto newtons_pt = au::QuantityPointMaker<Newtons> {};

namespace symbols {
constexpr auto N = au::SymbolFor<Newtons> {};
}

}  // namespace eclipse::units