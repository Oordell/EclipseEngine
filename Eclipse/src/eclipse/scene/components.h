#pragma once

#include "eclipse/scene/scene_camera.h"
#include "eclipse/renderer/texture.h"
#include "eclipse/core/uuid.h"
#include "eclipse/common_types/units/newtons.h"
#include "eclipse/common_types/units/densities.h"

#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <au.hh>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace eclipse::component {

struct ID {
	UUID id {};
};

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
		return glm::translate(IDENTITY_MATRIX, translation) * glm::toMat4(glm::quat(rotation)) *
		       glm::scale(IDENTITY_MATRIX, scale);
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
	ref<Texture2D> texture = nullptr;
	float tiling_factor    = 1.F;
};

struct CircleRenderer {
	CircleRenderer()                      = default;
	~CircleRenderer()                     = default;
	CircleRenderer(const CircleRenderer&) = default;

	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	au::QuantityF<au::Meters> radius {au::meters(0.5F)};

	// 1 = filled completely, 0.5 = filled half, 0.0 = infinite thin ring
	au::QuantityF<au::Unos> thickness {au::unos(1.0F)};

	// 1.0 = very blurry, 0.0 = Sharp
	au::QuantityF<au::Unos> fade {au::unos(0.005F)};
};

struct Camera {
	Camera()              = default;
	~Camera()             = default;
	Camera(const Camera&) = default;

	SceneCamera camera;
	bool primary {true};
	bool fixed_aspect_ratio {false};
};

struct RigidBody2D {
	static constexpr uint32_t NUM_OF_TYPES = 3;
	enum class BodyType { Static, Dynamic, Kinematic };
	static constexpr std::array<const char*, NUM_OF_TYPES> body_type_strings {"Static", "Dynamic", "Kinematic"};

	static std::string to_string(BodyType type) {
		static std::map<BodyType, std::string> conversion;
		using enum BodyType;
#define BODY_TYPE_TO_STRING(t) conversion.emplace(t, #t);
		BODY_TYPE_TO_STRING(Static);
		BODY_TYPE_TO_STRING(Dynamic);
		BODY_TYPE_TO_STRING(Kinematic);
#undef BODY_TYPE_TO_STRING
		return conversion[type];
	}

	static BodyType from_string(const std::string& type) {
		using enum BodyType;
		if (type == to_string(Static)) {
			return Static;
		} else if (type == to_string(Dynamic)) {
			return Dynamic;
		} else if (type == to_string(Kinematic)) {
			return Kinematic;
		}
		EC_CORE_ASSERT(false, "String not recognized as BodyType!: {0}", type);
		return Static;
	}

	BodyType type = BodyType::Static;
	bool fixed_rotation {false};

	// Storage for runtime
	void* runtime_body = nullptr;
};

struct BoxCollider2D {
	glm::vec2 offset {0.F, 0.F};
	glm::vec2 size {.5F, .5F};

	au::QuantityF<units::Densities> density {units::densities(1.F)};
	au::QuantityF<units::Newtons> friction {units::newtons(.5F)};
	au::QuantityF<au::Unos> restitution {au::unos(0.F)};
	au::QuantityF<au::Unos> restitution_threshold {au::unos(.5F)};

	// Storage for runtime
	void* runtime_fixture = nullptr;
};

struct CircleCollider2D {
	glm::vec2 offset {0.F, 0.F};
	au::QuantityF<au::Meters> radius {au::meters(0.5F)};

	au::QuantityF<units::Densities> density {units::densities(1.F)};
	au::QuantityF<units::Newtons> friction {units::newtons(.5F)};
	au::QuantityF<au::Unos> restitution {au::unos(0.F)};
	au::QuantityF<au::Unos> restitution_threshold {au::unos(.5F)};

	// Storage for runtime
	void* runtime_fixture = nullptr;
};

}  // namespace eclipse::component