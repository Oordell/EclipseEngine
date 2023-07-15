#include "ecpch.h"
#include "orthographic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {
OrthographicCamera::OrthographicCamera(const OrthograficLimits& args)
    : projection_matrix_(glm::ortho(args.left, args.right, args.bottom, args.top, args.near_plane, args.far_plane)),
      view_matrix_(1.0F) {
	EC_PROFILE_FUNCTION();

	calculate_view_projection_matrix();
}

void OrthographicCamera::set_projection(const OrthograficLimits& args) {
	EC_PROFILE_FUNCTION();

	projection_matrix_ = glm::ortho(args.left, args.right, args.bottom, args.top, args.near_plane, args.far_plane);
	calculate_view_projection_matrix();
}

void OrthographicCamera::calculate_view_matrix() {
	EC_PROFILE_FUNCTION();

	static const glm::mat4 identity_matrix = glm::mat4(1.0F);
	static const glm::vec3 rotation_axis   = glm::vec3(0, 0, 1);

	glm::mat4 transform =
	    glm::translate(identity_matrix, position_) * glm::rotate(identity_matrix, glm::radians(rotation_), rotation_axis);

	view_matrix_ = glm::inverse(transform);
	calculate_view_projection_matrix();
}

void OrthographicCamera::calculate_view_projection_matrix() {
	view_projection_matrix_ = projection_matrix_ * view_matrix_;
}
}  // namespace eclipse