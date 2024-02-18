#pragma once

#include <numbers>
#include <concepts>
#include <glm/glm.hpp>

#include "eclipse/common_types/transform.h"

namespace eclipse::utils {

std::optional<Transform> decompose_transform(const glm::mat4& raw_transform);

}  // namespace eclipse::utils