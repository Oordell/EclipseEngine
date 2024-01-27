#pragma once

#include "eclipse/core/core.h"

#include <glm/glm.hpp>

namespace eclipse {

struct ECLIPSE_API Transform {
	glm::vec3 translation {};
	glm::vec3 rotation {};
	glm::vec3 scale {};
};

}  // namespace eclipse