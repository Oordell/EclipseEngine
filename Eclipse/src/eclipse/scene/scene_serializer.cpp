#include "ecpch.h"
#include "scene_serializer.h"
#include "components.h"
#include "texture_sheets_components.h"
#include "entity.h"

#include <yaml-cpp/yaml.h>
#include <au.hh>

namespace YAML {

template <>
struct convert<glm::vec2> {
	static Node encode(const glm::vec2& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec2& rhs) {
		if (!node.IsSequence() || node.size() != 2) {
			return false;
		}
		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

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

Emitter& operator<<(Emitter& out, const glm::vec2& v) {
	out << Flow;
	out << BeginSeq << v.x << v.y << EndSeq;
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
	EC_CORE_ASSERT(entity.has_component<component::ID>(), "Entity does not have an UUID component!");

	out << YAML::BeginMap;
	out << YAML::Key << serializer_keys::ENTITY << YAML::Value << entity.get_uuid().value();

	if (entity.has_component<component::Tag>()) {
		auto& tag = entity.get_component<component::Tag>().tag;
		out << YAML::Key << serializer_keys::TAG_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::TAG_TAG << YAML::Value << tag;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::Transform>()) {
		auto& transform = entity.get_component<component::Transform>();
		out << YAML::Key << serializer_keys::TRANSFORM_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::TRANSFORM_TRANSLATION << YAML::Value << transform.translation;
		out << YAML::Key << serializer_keys::TRANSFORM_ROTATION << YAML::Value << transform.rotation;
		out << YAML::Key << serializer_keys::TRANSFORM_SCALE << YAML::Value << transform.scale;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::Camera>()) {
		auto& camera_component = entity.get_component<component::Camera>();
		auto& camera           = camera_component.camera;

		out << YAML::Key << serializer_keys::CAMERA_COMPONENT;
		out << YAML::BeginMap;

		out << YAML::Key << serializer_keys::CAMERA_CAMERA;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::CAMERA_PROJECTION_TYPE << YAML::Value
		    << static_cast<int>(camera.get_projection_type());
		out << YAML::Key << serializer_keys::CAMERA_PERSPECTIVE_VERTICAL_FOV << YAML::Value
		    << camera.get_perspective_vertical_fov().in(au::radians);
		out << YAML::Key << serializer_keys::CAMERA_PERSPECTIVE_NEAR << YAML::Value << camera.get_perspective_near_clip();
		out << YAML::Key << serializer_keys::CAMERA_PERSPECTIVE_FAR << YAML::Value << camera.get_perspective_far_clip();
		out << YAML::Key << serializer_keys::CAMERA_ORTHOGRAPHIC_SIZE << YAML::Value << camera.get_orthographic_size();
		out << YAML::Key << serializer_keys::CAMERA_ORTHOGRAPHIC_NEAR << YAML::Value << camera.get_orthographic_near_clip();
		out << YAML::Key << serializer_keys::CAMERA_ORTHOGRAPHIC_FAR << YAML::Value << camera.get_orthographic_far_clip();
		out << YAML::EndMap;

		out << YAML::Key << serializer_keys::CAMERA_PRIMARY << YAML::Value << camera_component.primary;
		out << YAML::Key << serializer_keys::CAMERA_FIXED_ASPECT_RATIO << YAML::Value << camera_component.fixed_aspect_ratio;

		out << YAML::EndMap;
	}

	if (entity.has_component<component::Color>()) {
		auto& color_component = entity.get_component<component::Color>();
		out << YAML::Key << serializer_keys::COLOR_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::COLOR_COLOR << YAML::Value << color_component.color;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::SpriteRenderer>()) {
		auto& sprite_renderer_component = entity.get_component<component::SpriteRenderer>();
		out << YAML::Key << serializer_keys::SPRITE_RENDERER_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::SPRITE_RENDERER_COLOR << YAML::Value << sprite_renderer_component.color;
		if (sprite_renderer_component.texture) {
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_PATH << YAML::Value
			    << sprite_renderer_component.texture->get_path().value_or("");
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_WIDTH << YAML::Value
			    << sprite_renderer_component.texture->get_width().in(units::pixels);
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_HEIGHT << YAML::Value
			    << sprite_renderer_component.texture->get_height().in(units::pixels);
		} else {
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_PATH << YAML::Value << "";
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_WIDTH << YAML::Value << 0;
			out << YAML::Key << serializer_keys::SPRITE_RENDERER_TEXTURE_HEIGHT << YAML::Value << 0;
		}
		out << YAML::Key << serializer_keys::SPRITE_RENDERER_TILING_FACTOR << YAML::Value
		    << sprite_renderer_component.tiling_factor;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::CircleRenderer>()) {
		auto& circle_component = entity.get_component<component::CircleRenderer>();
		out << YAML::Key << serializer_keys::CIRCLE_RENDERER_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::CIRCLE_RENDERER_COLOR << YAML::Value << circle_component.color;
		out << YAML::Key << serializer_keys::CIRCLE_RENDERER_RADIUS << YAML::Value << circle_component.radius.in(au::meters);
		out << YAML::Key << serializer_keys::CIRCLE_RENDERER_THICKNESS << YAML::Value
		    << circle_component.thickness.in(au::unos);
		out << YAML::Key << serializer_keys::CIRCLE_RENDERER_FADE << YAML::Value << circle_component.fade.in(au::unos);
		out << YAML::EndMap;
	}

	if (entity.has_component<component::RigidBody2D>()) {
		auto& rigid_body_2d_component = entity.get_component<component::RigidBody2D>();
		out << YAML::Key << serializer_keys::RIGID_BODY_2D_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::RIGID_BODY_2D_TYPE << YAML::Value
		    << component::RigidBody2D::to_string(rigid_body_2d_component.type);
		out << YAML::Key << serializer_keys::RIGID_BODY_2D_FIXED_ROTATION << YAML::Value
		    << rigid_body_2d_component.fixed_rotation;
		out << YAML::EndMap;
	}

	if (entity.has_component<component::BoxCollider2D>()) {
		auto& box_collider_2d_component = entity.get_component<component::BoxCollider2D>();
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_OFFSET << YAML::Value << box_collider_2d_component.offset;
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_SIZE << YAML::Value << box_collider_2d_component.size;
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_DENSITY << YAML::Value
		    << box_collider_2d_component.density.in(units::densities);
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_FRICTION << YAML::Value
		    << box_collider_2d_component.friction.in(units::newtons);
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_RESTITUTION << YAML::Value
		    << box_collider_2d_component.restitution.in(au::unos);
		out << YAML::Key << serializer_keys::BOX_COLLIDER_2D_RESTITUTION_THRESHOLD << YAML::Value
		    << box_collider_2d_component.restitution_threshold.in(au::unos);
		out << YAML::EndMap;
	}

	if (entity.has_component<component::CircleCollider2D>()) {
		auto& circle_collider_2d_component = entity.get_component<component::CircleCollider2D>();
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_OFFSET << YAML::Value << circle_collider_2d_component.offset;
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_RADIUS << YAML::Value
		    << circle_collider_2d_component.radius.in(au::meters);
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_DENSITY << YAML::Value
		    << circle_collider_2d_component.density.in(units::densities);
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_FRICTION << YAML::Value
		    << circle_collider_2d_component.friction.in(units::newtons);
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_RESTITUTION << YAML::Value
		    << circle_collider_2d_component.restitution.in(au::unos);
		out << YAML::Key << serializer_keys::CIRCLE_COLLIDER_2D_RESTITUTION_THRESHOLD << YAML::Value
		    << circle_collider_2d_component.restitution_threshold.in(au::unos);
		out << YAML::EndMap;
	}

	if (entity.has_component<component::TextureSheetComponent>()) {
		auto& texture_sheet_component = entity.get_component<component::TextureSheetComponent>();
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_TEXTURE_PATH << YAML::Value
		    << texture_sheet_component.texture_sheet->get_texture()->get_path().value_or("");
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_SUB_TILE_WIDTH << YAML::Value
		    << texture_sheet_component.texture_sheet->get_tile_width_in_pixels().in(units::pixels);
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_SUB_TILE_HEIGHT << YAML::Value
		    << texture_sheet_component.texture_sheet->get_tile_height_in_pixels().in(units::pixels);
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_SUB_TILE_SPACING_X << YAML::Value
		    << texture_sheet_component.texture_sheet->get_tile_spacing_x_in_pixels().in(units::pixels);
		out << YAML::Key << serializer_keys::TEXTURE_SHEET_SUB_TILE_SPACING_Y << YAML::Value
		    << texture_sheet_component.texture_sheet->get_tile_spacing_y_in_pixels().in(units::pixels);
		out << YAML::EndMap;
	}

	if (entity.has_component<component::SubTexture>()) {
		auto& sub_texture_component = entity.get_component<component::SubTexture>();
		out << YAML::Key << serializer_keys::SUB_TEXTURE_COMPONENT;
		out << YAML::BeginMap;
		out << YAML::Key << serializer_keys::SUB_TEXTURE_TEXTURE_PATH << YAML::Value
		    << sub_texture_component.sub_texture->get_texture()->get_path().value_or("");
		out << YAML::Key << serializer_keys::SUB_TEXTURE_TILE_INDEX_X << YAML::Value
		    << sub_texture_component.sub_texture->get_index_x().in(units::pixels);
		out << YAML::Key << serializer_keys::SUB_TEXTURE_TILE_INDEX_Y << YAML::Value
		    << sub_texture_component.sub_texture->get_index_y().in(units::pixels);
		out << YAML::Key << serializer_keys::SUB_TEXTURE_TILE_WIDTH << YAML::Value
		    << sub_texture_component.sub_texture->get_width().in(units::pixels);
		out << YAML::Key << serializer_keys::SUB_TEXTURE_TILE_HEIGHT << YAML::Value
		    << sub_texture_component.sub_texture->get_height().in(units::pixels);
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void SceneSerializer::serialize_text(const std::filesystem::path& file_path) {
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

	std::ofstream fout(file_path.string());
	fout << out.c_str();
}

void SceneSerializer::serialize_binary(const std::filesystem::path& file_path) {
	EC_CORE_ASSERT(false, "Binary serialization is not implemented yet!");
}

bool SceneSerializer::deserialize_text(const std::filesystem::path& file_path) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(file_path.string());
	} catch (YAML::ParserException& e) {
		EC_CORE_ERROR("Error: {0}", e.msg);
		return false;
	}

	if (!data[serializer_keys::SCENE]) {
		EC_CORE_ERROR("Error loading eclipse file. Couldn't identify Scene...");
		return false;
	}

	std::string scene_name = data[serializer_keys::SCENE].as<std::string>();
	EC_CORE_TRACE("Deserializing scene: \"{0}\"", scene_name);

	auto entities = data[serializer_keys::ENTITIES];
	if (!entities) {
		EC_CORE_DEBUG("Scene \"{0}\" didn't contain any entities.", scene_name);
		return true;
	}

	struct EntityCollection {
		Entity eclipse_entity;
		YAML::iterator::value_type yaml_entity;
	};

	std::vector<EntityCollection> entities_with_sub_textures;
	std::vector<ref<TextureSheet>> texture_sheets_loaded;

	for (auto e : entities) {
		UUID uuid = UUID(e[serializer_keys::ENTITY].as<uint64_t>());

		std::string name {};
		auto tag_component = e[serializer_keys::TAG_COMPONENT];
		if (tag_component) {
			name = tag_component[serializer_keys::TAG_TAG].as<std::string>();
		}
		EC_CORE_DEBUG("Deserialized entity with ID \"{0}\", name \"{1}\"", uuid.value(), name);

		Entity deserialized_entity = scene_->create_entity_from_uuid(uuid, name);

		auto transform_component = e[serializer_keys::TRANSFORM_COMPONENT];
		if (transform_component) {
			// Entities always have transform components!
			auto& tc       = deserialized_entity.get_component<component::Transform>();
			tc.translation = transform_component[serializer_keys::TRANSFORM_TRANSLATION].as<glm::vec3>();
			tc.rotation    = transform_component[serializer_keys::TRANSFORM_ROTATION].as<glm::vec3>();
			tc.scale       = transform_component[serializer_keys::TRANSFORM_SCALE].as<glm::vec3>();
		}

		auto camera_component = e[serializer_keys::CAMERA_COMPONENT];
		if (camera_component) {
			auto& cc = deserialized_entity.add_component<component::Camera>();

			auto camera_props = camera_component[serializer_keys::CAMERA_CAMERA];
			cc.camera.set_projection_type(
			    static_cast<ProjectionType>(camera_props[serializer_keys::CAMERA_PROJECTION_TYPE].as<int>()));

			cc.camera.set_perspective_vertical_fov(
			    au::radians(camera_props[serializer_keys::CAMERA_PERSPECTIVE_VERTICAL_FOV].as<float>()));
			cc.camera.set_perspective_near_clip(camera_props[serializer_keys::CAMERA_PERSPECTIVE_NEAR].as<float>());
			cc.camera.set_perspective_far_clip(camera_props[serializer_keys::CAMERA_PERSPECTIVE_FAR].as<float>());

			cc.camera.set_orthographic_size(camera_props[serializer_keys::CAMERA_ORTHOGRAPHIC_SIZE].as<float>());
			cc.camera.set_orthographic_near_clip(camera_props[serializer_keys::CAMERA_ORTHOGRAPHIC_NEAR].as<float>());
			cc.camera.set_orthographic_far_clip(camera_props[serializer_keys::CAMERA_ORTHOGRAPHIC_FAR].as<float>());

			cc.primary            = camera_component[serializer_keys::CAMERA_PRIMARY].as<bool>();
			cc.fixed_aspect_ratio = camera_component[serializer_keys::CAMERA_FIXED_ASPECT_RATIO].as<bool>();
		}

		auto color_component = e[serializer_keys::COLOR_COMPONENT];
		if (color_component) {
			auto& color = deserialized_entity.add_component<component::Color>();
			color.color = color_component[serializer_keys::COLOR_COLOR].as<glm::vec4>();
		}

		auto sprite_renderer_component = e[serializer_keys::SPRITE_RENDERER_COMPONENT];
		if (sprite_renderer_component) {
			auto& src         = deserialized_entity.add_component<component::SpriteRenderer>();
			src.color         = sprite_renderer_component[serializer_keys::SPRITE_RENDERER_COLOR].as<glm::vec4>();
			src.tiling_factor = sprite_renderer_component[serializer_keys::SPRITE_RENDERER_TILING_FACTOR].as<float>();
			std::string texture_path =
			    sprite_renderer_component[serializer_keys::SPRITE_RENDERER_TEXTURE_PATH].as<std::string>();
			uint32_t texture_width  = sprite_renderer_component[serializer_keys::SPRITE_RENDERER_TEXTURE_WIDTH].as<uint32_t>();
			uint32_t texture_height = sprite_renderer_component[serializer_keys::SPRITE_RENDERER_TEXTURE_HEIGHT].as<uint32_t>();
			ref<Texture2D> texture  = nullptr;
			if (!texture_path.empty()) {
				texture = Texture2D::create(texture_path);
			} else if (texture_width != 0 && texture_height != 0) {
				texture = Texture2D::create({.width = units::pixels(texture_width), .height = units::pixels(texture_height)});
			}
			src.texture = texture;
		}

		auto circle_component = e[serializer_keys::CIRCLE_RENDERER_COMPONENT];
		if (circle_component) {
			auto& src     = deserialized_entity.add_component<component::CircleRenderer>();
			src.color     = circle_component[serializer_keys::CIRCLE_RENDERER_COLOR].as<glm::vec4>();
			src.radius    = au::meters(circle_component[serializer_keys::CIRCLE_RENDERER_RADIUS].as<float>());
			src.thickness = au::unos(circle_component[serializer_keys::CIRCLE_RENDERER_THICKNESS].as<float>());
			src.fade      = au::unos(circle_component[serializer_keys::CIRCLE_RENDERER_FADE].as<float>());
		}

		auto rigid_body_2d_component = e[serializer_keys::RIGID_BODY_2D_COMPONENT];
		if (rigid_body_2d_component) {
			auto& src = deserialized_entity.add_component<component::RigidBody2D>();
			src.type  = component::RigidBody2D::from_string(
       rigid_body_2d_component[serializer_keys::RIGID_BODY_2D_TYPE].as<std::string>());
			src.fixed_rotation = rigid_body_2d_component[serializer_keys::RIGID_BODY_2D_FIXED_ROTATION].as<bool>();
		}

		auto box_collider_2d_component = e[serializer_keys::BOX_COLLIDER_2D_COMPONENT];
		if (box_collider_2d_component) {
			auto& src   = deserialized_entity.add_component<component::BoxCollider2D>();
			src.offset  = box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_OFFSET].as<glm::vec2>();
			src.size    = box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_SIZE].as<glm::vec2>();
			src.density = (au::kilo(au::grams) / au::cubed(au::meters))(
			    box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_DENSITY].as<float>());
			src.friction = (au::kilo(au::grams) * au::meters / au::squared(au::seconds))(
			    box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_FRICTION].as<float>());
			src.restitution = au::unos(box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_RESTITUTION].as<float>());
			src.restitution_threshold =
			    au::unos(box_collider_2d_component[serializer_keys::BOX_COLLIDER_2D_RESTITUTION_THRESHOLD].as<float>());
		}

		auto circle_collider_2d_component = e[serializer_keys::CIRCLE_COLLIDER_2D_COMPONENT];
		if (circle_collider_2d_component) {
			auto& src   = deserialized_entity.add_component<component::CircleCollider2D>();
			src.offset  = circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_OFFSET].as<glm::vec2>();
			src.radius  = au::meters(circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_RADIUS].as<float>());
			src.density = (au::kilo(au::grams) / au::cubed(au::meters))(
			    circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_DENSITY].as<float>());
			src.friction = (au::kilo(au::grams) * au::meters / au::squared(au::seconds))(
			    circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_FRICTION].as<float>());
			src.restitution =
			    au::unos(circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_RESTITUTION].as<float>());
			src.restitution_threshold =
			    au::unos(circle_collider_2d_component[serializer_keys::CIRCLE_COLLIDER_2D_RESTITUTION_THRESHOLD].as<float>());
		}

		auto texture_sheet_component = e[serializer_keys::TEXTURE_SHEET_COMPONENT];
		if (texture_sheet_component) {
			auto& src = deserialized_entity.add_component<component::TextureSheetComponent>();
			au::Quantity<units::Pixels, uint32_t> sub_tile_width =
			    units::pixels(texture_sheet_component[serializer_keys::TEXTURE_SHEET_SUB_TILE_WIDTH].as<int>());
			au::Quantity<units::Pixels, uint32_t> sub_tile_height =
			    units::pixels(texture_sheet_component[serializer_keys::TEXTURE_SHEET_SUB_TILE_HEIGHT].as<int>());
			au::Quantity<units::Pixels, uint32_t> sub_tile_spacing_x =
			    units::pixels(texture_sheet_component[serializer_keys::TEXTURE_SHEET_SUB_TILE_SPACING_X].as<int>());
			au::Quantity<units::Pixels, uint32_t> sub_tile_spacing_y =
			    units::pixels(texture_sheet_component[serializer_keys::TEXTURE_SHEET_SUB_TILE_SPACING_Y].as<int>());

			std::string texture_path = texture_sheet_component[serializer_keys::TEXTURE_SHEET_TEXTURE_PATH].as<std::string>();
			ref<Texture2D> texture   = nullptr;
			if (!texture_path.empty()) {
				texture = Texture2D::create(texture_path);
			} else {
				texture = Texture2D::create({.width = units::pixels(10), .height = units::pixels(10)});
			}
			src.texture_sheet = make_ref<TextureSheet>(TextureSheetProperties {.texture            = texture,
			                                                                   .sub_tile_width     = sub_tile_width,
			                                                                   .sub_tile_height    = sub_tile_height,
			                                                                   .sub_tile_spacing_x = sub_tile_spacing_x,
			                                                                   .sub_tile_spacing_y = sub_tile_spacing_y});
			texture_sheets_loaded.push_back(src.texture_sheet);
		}

		// We need to first load all the texture sheets, before creating the sub textures, as they have pointers to the
		// texture sheets. Thus, we save all the entities with sub textures for now, and create them in a seperate loop after
		// this.
		auto sub_texture_component = e[serializer_keys::SUB_TEXTURE_COMPONENT];
		if (sub_texture_component) {
			entities_with_sub_textures.push_back({deserialized_entity, e});
		}
	}

	// Now that all texture sheets have been loaded, we can create sub textures:
	for (auto [eclipse_entity, yaml_entity] : entities_with_sub_textures) {
		auto sub_texture_component     = yaml_entity[serializer_keys::SUB_TEXTURE_COMPONENT];
		std::string texture_sheet_path = sub_texture_component[serializer_keys::SUB_TEXTURE_TEXTURE_PATH].as<std::string>();
		for (auto texture_sheet : texture_sheets_loaded) {
			if (texture_sheet->get_texture()->get_path() == texture_sheet_path) {
				auto& src = eclipse_entity.add_component<component::SubTexture>();

				au::Quantity<units::Pixels, uint32_t> tile_index_x =
				    units::pixels(sub_texture_component[serializer_keys::SUB_TEXTURE_TILE_INDEX_X].as<int>());
				au::Quantity<units::Pixels, uint32_t> tile_index_y =
				    units::pixels(sub_texture_component[serializer_keys::SUB_TEXTURE_TILE_INDEX_Y].as<int>());
				au::Quantity<units::Pixels, uint32_t> tile_width =
				    units::pixels(sub_texture_component[serializer_keys::SUB_TEXTURE_TILE_WIDTH].as<int>());
				au::Quantity<units::Pixels, uint32_t> tile_height =
				    units::pixels(sub_texture_component[serializer_keys::SUB_TEXTURE_TILE_HEIGHT].as<int>());

				src.sub_texture = make_ref<SubTexture2D>(SubTexture2DProperties {.texture_sheet = texture_sheet,
				                                                                 .tile_index_x  = tile_index_x,
				                                                                 .tile_index_y  = tile_index_y,
				                                                                 .tile_width    = tile_width,
				                                                                 .tile_height   = tile_height});
			}
		}
	}

	return true;
}

bool SceneSerializer::deserialize_binary(const std::filesystem::path& file_path) {
	EC_CORE_ASSERT(false, "Binary deserialization is not implemented yet!");
	return false;
}
}  // namespace eclipse