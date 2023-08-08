#pragma once

#include <glm/glm.hpp>

namespace eclipse {

class Camera {
public:
	Camera() = default;

	Camera(const glm::mat4& projection) : projection_(projection) {}

	virtual ~Camera() = default;

	const glm::mat4& get_projection() const { return projection_; }

protected:
	glm::mat4 projection_ {1.0F};
};

}  // namespace eclipse