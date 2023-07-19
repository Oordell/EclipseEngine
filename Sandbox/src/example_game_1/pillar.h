#pragma once

#include <glm/glm.hpp>

struct Pillar {
	glm::vec3 top_position    = {0.0F, 10.0F, 0.0F};
	glm::vec2 top_scale       = {15.0F, 20.0F};
	glm::vec3 bottom_position = {10.0F, 10.0F, 0.0F};
	glm::vec2 bottom_scale    = {15.0F, 20.0F};
};