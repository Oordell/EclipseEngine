#pragma once

#include "eclipse/scene/scene_camera.h"
#include "eclipse/scene/scriptable_entity.h"
#include "eclipse/core/timestep.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	glm::mat4 get_transform() const {
		static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);

		glm::mat4 rotation_mat = glm::rotate(IDENTITY_MATRIX, rotation.x, {1, 0, 0}) *
		                         glm::rotate(IDENTITY_MATRIX, rotation.y, {0, 1, 0}) *
		                         glm::rotate(IDENTITY_MATRIX, rotation.z, {0, 0, 1});
		return glm::translate(IDENTITY_MATRIX, translation) * rotation_mat * glm::scale(IDENTITY_MATRIX, scale);
	}

	glm::vec3 translation {0.0F, 0.0F, 0.0F};
	glm::vec3 rotation {0.0F, 0.0F, 0.0F};
	glm::vec3 scale {1.0F, 1.0F, 1.0F};
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