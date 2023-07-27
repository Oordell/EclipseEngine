#pragma once

#include <numbers>
#include <concepts>

namespace eclipse::utils {

template <typename T>
concept floating_point = std::is_floating_point_v<T>;

floating_point auto deg_to_rad(const floating_point auto& angle) {
	return angle * (static_cast<float>(std::numbers::pi) / 180.0F);
}

floating_point auto rad_to_deg(const floating_point auto& angle) {
	return angle * (180.0F / static_cast<float>(std::numbers::pi));
}

}  // namespace eclipse::utils