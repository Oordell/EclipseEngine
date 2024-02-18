#pragma once

#include <concepts>

namespace eclipse::utils::num {

template <class Type>
inline constexpr bool eclipse_false = false;

template <class Type>
struct IllFormed {
	static_assert(eclipse_false<Type>,
	              "A program that instantiates a primary template of a mathematical constant variable template is "
	              "ill-formed. (N4950 [math.constants]/3)");
};

/* clang-format off */
template <class Type> inline constexpr Type DEGREES_180_V = IllFormed<Type> {};
template <class Type> inline constexpr Type DEGREES_90_V = IllFormed<Type> {};

template <std::floating_point Type> inline constexpr Type DEGREES_180_V<Type> = 180.0F;
template <std::floating_point Type> inline constexpr Type DEGREES_90_V<Type> = 90.0F;
/* clang-format on */

inline constexpr double DEGREES_180   = DEGREES_180_V<double>;
inline constexpr double DEGREES_180_F = DEGREES_180_V<float>;
inline constexpr double DEGREES_90    = DEGREES_90_V<double>;
inline constexpr double DEGREES_90_F  = DEGREES_90_V<float>;

}  // namespace eclipse::utils::num