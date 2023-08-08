#pragma once

#include "eclipse/renderer/camera.h"

#include <glm/glm.hpp>

namespace eclipse::component {

struct Tag {
	Tag()           = default;
	~Tag()          = default;
	Tag(const Tag&) = default;

	Tag(const std::string& tag) : tag(tag) {}

	std::string tag;
};

struct Transform {
	Transform()                 = default;
	~Transform()                = default;
	Transform(const Transform&) = default;

	Transform(const glm::mat4& trans) : transform(trans) {}

	operator glm::mat4() { return transform; }

	operator const glm::mat4&() const { return transform; }

	glm::mat4 transform {1.0F};
};

struct Color {
	Color()             = default;
	~Color()            = default;
	Color(const Color&) = default;

	Color(const glm::vec4& color) : color(color) {}

	operator glm::vec4() { return color; }

	operator const glm::vec4&() const { return color; }

	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
};

struct Camera {
	Camera()              = default;
	~Camera()             = default;
	Camera(const Camera&) = default;

	Camera(const eclipse::Camera& camera) : camera(camera) {}

	eclipse::Camera camera;
	bool primary {true};
};

}  // namespace eclipse::component