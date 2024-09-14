#pragma once

#include <concepts>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif  // !GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "eclipse/common_types/transform.h"
#include "eclipse/common_types/pose_2d.h"

namespace eclipse::utils {

std::optional<Transform> decompose_transform(const glm::mat4& raw_transform);

inline static constexpr glm::mat4 create_transform(const glm::vec3& translation, const glm::vec3& rotation,
                                                   const glm::vec3& scale) {
	constexpr glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	return glm::translate(IDENTITY_MATRIX, translation) * glm::toMat4(glm::quat(rotation)) *
	       glm::scale(IDENTITY_MATRIX, scale);
}

inline static constexpr glm::mat4 create_transform_with_offset(const glm::vec3& translation, const glm::vec3& rotation,
                                                               const glm::vec3& scale, const glm::vec2& offset) {
	constexpr glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	return glm::translate(IDENTITY_MATRIX, translation) * glm::toMat4(glm::quat(rotation)) *
	       glm::translate(IDENTITY_MATRIX, glm::vec3(offset, 0.001F)) * glm::scale(IDENTITY_MATRIX, scale);
}

inline static constexpr Pose2D transform_pose2d(const Pose2D& pose, const Pose2D& transform) {
	const auto th    = transform.theta.in<au::Radians>();
	const auto x_rot = pose.x * std::cos(th) - pose.y * std::sin(th);
	const auto y_rot = pose.x * std::sin(th) + pose.y * std::cos(th);

	const auto x_new     = x_rot + transform.x;
	const auto y_new     = y_rot + transform.y;
	const auto theta_new = pose.theta + transform.theta;

	return {.x = x_new, .y = y_new, .theta = theta_new};
}

inline static constexpr Pose2D inverse_transform_pose2d(const Pose2D& pose, const Pose2D& transform) {
	const auto x_translated = pose.x - transform.x;
	const auto y_translated = pose.y - transform.y;

	const auto th    = transform.theta.in<au::Radians>();
	const auto x_new = x_translated * std::cos(-th) - y_translated * std::sin(-th);
	const auto y_new = x_translated * std::sin(-th) + y_translated * std::cos(-th);

	const auto theta_new = pose.theta - transform.theta;

	return {.x = x_new, .y = y_new, .theta = theta_new};
}

}  // namespace eclipse::utils