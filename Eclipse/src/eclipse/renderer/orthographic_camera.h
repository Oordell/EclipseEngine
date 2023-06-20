#pragma once

#include "eclipse/common_types/orthographic.h"

#include <glm/glm.hpp>

namespace eclipse {

/**
 * @brief A 2D camera
 */
class OrthographicCamera {
public:
	OrthographicCamera(const OrthograficLimits& args);

	void set_projection(const OrthograficLimits& args);

	const glm::vec3& get_position() const { return position_; }

	float get_rotation() const { return rotation_; }

	void set_position(const glm::vec3& position) {
		position_ = position;
		calculate_view_matrix();
	}

	void set_rotation(float rotation) {
		rotation_ = rotation;
		calculate_view_matrix();
	}

	const glm::mat4& get_projection_matrix() const { return projection_matrix_; }

	const glm::mat4& get_view_matrix() const { return view_matrix_; }

	const glm::mat4& get_view_projection_matrix() const { return view_projection_matrix_; }

private:
	void calculate_view_matrix();
	void calculate_view_projection_matrix();

	glm::mat4 projection_matrix_ {};
	glm::mat4 view_matrix_ {};
	glm::mat4 view_projection_matrix_ {};
	glm::vec3 position_ {0, 0, 0};
	float rotation_ {0.0F};
};

}  // namespace eclipse