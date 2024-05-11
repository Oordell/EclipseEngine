#pragma once

#include <numbers>
#include <concepts>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif  // !GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "eclipse/common_types/transform.h"

namespace eclipse::utils {

std::optional<Transform> decompose_transform(const glm::mat4& raw_transform);

inline constexpr glm::mat4 create_transform(const glm::vec3& translation, const glm::vec3& rotation,
                                            const glm::vec3& scale) {
	constexpr glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	return glm::translate(IDENTITY_MATRIX, translation) * glm::toMat4(glm::quat(rotation)) *
	       glm::scale(IDENTITY_MATRIX, scale);
}

}  // namespace eclipse::utils