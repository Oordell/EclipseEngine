#pragma once

#include <numbers>
#include <concepts>
#include <glm/glm.hpp>

#include "eclipse/common_types/transform.h"

namespace eclipse::utils {

std::floating_point auto deg_to_rad(const std::floating_point auto& angle) {
	return angle * (static_cast<float>(std::numbers::pi) / 180.0F);
}

std::floating_point auto rad_to_deg(const std::floating_point auto& angle) {
	return angle * (180.0F / static_cast<float>(std::numbers::pi));
}

std::optional<Transform> decompose_transform(const glm::mat4& raw_transform);

}  // namespace eclipse::utils