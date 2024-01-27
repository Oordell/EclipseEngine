#include "ecpch.h"
#include "scene_serializer.h"
#include "components.h"
#include "entity.h"

#include <yaml-cpp/yaml.h>

namespace YAML {

template <>
struct convert<glm::vec3> {
	static Node encode(const glm::vec3& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec3& rhs) {
		if (!node.IsSequence() || node.size() != 3) {
			return false;
		}
		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template <>
struct convert<glm::vec4> {
	static Node encode(const glm::vec4& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec4& rhs) {
		if (!node.IsSequence() || node.size() != 4) {
			return false;
		}
		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

Emitter& operator<<(Emitter& out, const std::string_view& v) {
	out << v.data();
	return out;
}

Emitter& operator<<(Emitter& out, const glm::vec3& v) {
	out << Flow;
	out << BeginSeq << v.x << v.y << v.z << EndSeq;
	return out;
}

Emitter& operator<<(Emitter& out, const glm::vec4& v) {
	out << Flow;
	out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
	return out;
}
}  // namespace YAML

namespace eclipse {

SceneSerializer::SceneSerializer(const ref<Scene>& scene) : scene_(scene) {}

static void serialize_entity(YAML::Emitter& out, Entity entity) {
	out << YAML::BeginMap;
	out << YAML::Key << serializer_keys::ENTITY << YAML::Value << static_cast<uint32_t>(entity);

	if (entity.has_component<component::Tag>()) {
		auto& tag = entity.get_component<component::Tag>().tag;
		out << YAML::Key << serializer_keys::TAG_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::TAG << YAML::Value << tag;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::Transform>()) {
		auto& transform = entity.get_component<component::Transform>();
		out << YAML::Key << serializer_keys::TRANSFORM_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::TRANSLATION << YAML::Value << transform.translation;
		out << YAML::Key << serializer_keys::ROTATION << YAML::Value << transform.rotation;
		out << YAML::Key << serializer_keys::SCALE << YAML::Value << transform.scale;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::Camera>()) {
		auto& camera_component = entity.get_component<component::Camera>();
		auto& camera           = camera_component.camera;

		out << YAML::Key << serializer_keys::CAMERA_COMPONENT;
		out << YAML::BeginMap;

		out << YAML::Key << serializer_keys::CAMERA;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::PROJECTION_TYPE << YAML::Value << static_cast<int>(camera.get_projection_type());
		out << YAML::Key << serializer_keys::PERSPECTIVE_VERTICAL_FOV << YAML::Value << camera.get_perspective_vertical_fov();
		out << YAML::Key << serializer_keys::PERSPECTIVE_NEAR << YAML::Value << camera.get_perspective_near_clip();
		out << YAML::Key << serializer_keys::PERSPECTIVE_FAR << YAML::Value << camera.get_perspective_far_clip();
		out << YAML::Key << serializer_keys::ORTHOGRAPHIC_SIZE << YAML::Value << camera.get_orthographic_size();
		out << YAML::Key << serializer_keys::ORTHOGRAPHIC_NEAR << YAML::Value << camera.get_orthographic_near_clip();
		out << YAML::Key << serializer_keys::ORTHOGRAPHIC_FAR << YAML::Value << camera.get_orthographic_far_clip();
		out << YAML::EndMap;

		out << YAML::Key << serializer_keys::PRIMARY << YAML::Value << camera_component.primary;
		out << YAML::Key << serializer_keys::FIXED_ASPECT_RATIO << YAML::Value << camera_component.fixed_aspect_ratio;

		out << YAML::EndMap;
	}

	if (entity.has_component<component::Color>()) {
		auto& color_component = entity.get_component<component::Color>();
		out << YAML::Key << serializer_keys::COLOR_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::COLOR << YAML::Value << color_component.color;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::SpriteRenderer>()) {
		auto& sprite_renderer_component = entity.get_component<component::SpriteRenderer>();
		out << YAML::Key << serializer_keys::SPRITE_RENDERER_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::COLOR << YAML::Value << sprite_renderer_component.color;
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void SceneSerializer::serialize_text(const FilePath& file_path) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << serializer_keys::SCENE << YAML::Value << "Untitled scene";
	out << YAML::Key << serializer_keys::ENTITIES << YAML::Value << YAML::BeginSeq;
	scene_->get_registry().each([&](auto entity_id) {
		Entity entity {entity_id, scene_.get()};
		if (!entity) {
			return;
		}
		serialize_entity(out, entity);
	});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(file_path.value());
	fout << out.c_str();
}

void SceneSerializer::serialize_binary(const FilePath& file_path) {
	EC_CORE_ASSERT(false, "Binary serialization is not implemented yet!");
}

bool SceneSerializer::deserialize_text(const FilePath& file_path) {
	YAML::Node data = YAML::LoadFile(file_path.value());
	if (!data[serializer_keys::SCENE]) {
		EC_CORE_ERROR("Error loading eclipse file. Could identify Scene...");
		return false;
	}

	std::string scene_name = data[serializer_keys::SCENE].as<std::string>();
	EC_CORE_TRACE("Deserializing scene: \"{0}\"", scene_name);

	auto entities = data[serializer_keys::ENTITIES];
	if (!entities) {
		EC_CORE_DEBUG("Scene \"{0}\" didn't contain any entities.", scene_name);
		return true;
	}

	for (auto e : entities) {
		uint64_t uuid = e[serializer_keys::ENTITY].as<uint64_t>();

		std::string name {};
		auto tag_component = e[serializer_keys::TAG_COMPONENT];
		if (tag_component) {
			name = tag_component[serializer_keys::TAG].as<std::string>();
		}
		EC_CORE_DEBUG("Deserialized entity with ID \"{0}\", name \"{1}\"", uuid, name);

		Entity deserialized_entity = scene_->create_entity(name);

		auto transform_component = e[serializer_keys::TRANSFORM_COMPONENT];
		if (transform_component) {
			// Entities always have transform components!
			auto& tc       = deserialized_entity.get_component<component::Transform>();
			tc.translation = transform_component[serializer_keys::TRANSLATION].as<glm::vec3>();
			tc.rotation    = transform_component[serializer_keys::ROTATION].as<glm::vec3>();
			tc.scale       = transform_component[serializer_keys::SCALE].as<glm::vec3>();
		}

		auto camera_component = e[serializer_keys::CAMERA_COMPONENT];
		if (camera_component) {
			auto& cc = deserialized_entity.add_component<component::Camera>();

			auto camera_props = camera_component[serializer_keys::CAMERA];
			cc.camera.set_projection_type(static_cast<ProjectionType>(camera_props[serializer_keys::PROJECTION_TYPE].as<int>()));

			cc.camera.set_perspective_vertical_fov(camera_props[serializer_keys::PERSPECTIVE_VERTICAL_FOV].as<float>());
			cc.camera.set_perspective_near_clip(camera_props[serializer_keys::PERSPECTIVE_NEAR].as<float>());
			cc.camera.set_perspective_far_clip(camera_props[serializer_keys::PERSPECTIVE_FAR].as<float>());

			cc.camera.set_orthographic_size(camera_props[serializer_keys::ORTHOGRAPHIC_SIZE].as<float>());
			cc.camera.set_orthographic_near_clip(camera_props[serializer_keys::ORTHOGRAPHIC_NEAR].as<float>());
			cc.camera.set_orthographic_far_clip(camera_props[serializer_keys::ORTHOGRAPHIC_FAR].as<float>());

			cc.primary            = camera_component[serializer_keys::PRIMARY].as<bool>();
			cc.fixed_aspect_ratio = camera_component[serializer_keys::FIXED_ASPECT_RATIO].as<bool>();
		}

		auto color_component = e[serializer_keys::COLOR_COMPONENT];
		if (color_component) {
			auto& color = deserialized_entity.add_component<component::Color>();
			color.color = color_component[serializer_keys::COLOR].as<glm::vec4>();
		}

		auto sprite_renderer_component = e[serializer_keys::SPRITE_RENDERER_COMPONENT];
		if (sprite_renderer_component) {
			auto& src = deserialized_entity.add_component<component::SpriteRenderer>();
			src.color = sprite_renderer_component[serializer_keys::COLOR].as<glm::vec4>();
		}
	}

	return true;
}

bool SceneSerializer::deserialize_binary(const FilePath& file_path) {
	EC_CORE_ASSERT(false, "Binary deserialization is not implemented yet!");
	return false;
}
}  // namespace eclipse