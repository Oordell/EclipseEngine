#pragma once

#include <glm/glm.hpp>

namespace eclipse {

class Camera {
public:
	Camera(const glm::mat4& projection) : projection_(projection) {}

	const glm::mat4& get_projection() const { return projection_; }

private:
	glm::mat4 projection_ {1.0F};
};

}  // namespace eclipse