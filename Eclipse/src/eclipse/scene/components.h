#pragma once

#include "eclipse/scene/scene_camera.h"
#include "eclipse/scene/scriptable_entity.h"
#include "eclipse/core/timestep.h"

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

struct SpriteRenderer {
	SpriteRenderer()                      = default;
	~SpriteRenderer()                     = default;
	SpriteRenderer(const SpriteRenderer&) = default;

	SpriteRenderer(const glm::vec4& color) : color(color) {}

	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
};

struct Camera {
	Camera()              = default;
	~Camera()             = default;
	Camera(const Camera&) = default;

	SceneCamera camera;
	bool primary {true};
	bool fixed_aspect_ratio {false};
};

template <typename T>
concept ScriptableType = std::derived_from<T, ScriptableEntity>;

struct NativeScript {
	scope<ScriptableEntity> instance = nullptr;

	scope<ScriptableEntity> (*instantiate_script_func)();
	void (*destroy_script_func)(NativeScript*);

	template <ScriptableType T>
	void bind() {
		instantiate_script_func = []() { return static_cast<scope<ScriptableEntity>>(make_scope<T>()); };
		/* clang-format off */
		destroy_script_func     = [](NativeScript* ns) {
			ns->instance.reset();
			ns->instance = nullptr;
		};
		/* clang-format on */
	}
};

}  // namespace eclipse::component