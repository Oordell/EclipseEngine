#pragma once
#include <au.hh>

namespace eclipse::units {

struct Densities : decltype(au::Kilo<au::Grams> {} / au::cubed(au::Meters {})) {
	static constexpr inline const char label[] = "kg / m^3";
};

constexpr auto density      = au::SingularNameFor<Densities> {};
constexpr auto densities    = au::QuantityMaker<Densities> {};
constexpr auto densities_pt = au::QuantityPointMaker<Densities> {};

namespace symbols {
constexpr auto dens = au::SymbolFor<Densities> {};
}

}  // namespace eclipse::units