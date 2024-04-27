#pragma once

#include "scene.h"
#include "eclipse/common_types/file_path.h"

namespace eclipse {

class ECLIPSE_API SceneSerializer {
public:
	SceneSerializer(const ref<Scene>& scene);
	void serialize_text(const FilePath& file_path);
	void serialize_binary(const FilePath& file_path);
	bool deserialize_text(const FilePath& file_path);
	bool deserialize_binary(const FilePath& file_path);

private:
	ref<Scene> scene_;
};

namespace serializer_keys {
// Scene
static constexpr std::string_view SCENE    = "scene";
static constexpr std::string_view ENTITIES = "entities";
static constexpr std::string_view ENTITY   = "entity";

// component::Tag
static constexpr std::string_view TAG_COMPONENT = "tag_component";
static constexpr std::string_view TAG           = "tag";

// component::Transform
static constexpr std::string_view TRANSFORM_COMPONENT = "transform_component";
static constexpr std::string_view TRANSLATION         = "translation";
static constexpr std::string_view ROTATION            = "rotation";
static constexpr std::string_view SCALE               = "scale";

// component::Camera
static constexpr std::string_view CAMERA_COMPONENT         = "camera_component";
static constexpr std::string_view CAMERA                   = "camera";
static constexpr std::string_view PROJECTION_TYPE          = "projection_type";
static constexpr std::string_view PERSPECTIVE_VERTICAL_FOV = "perspective_vertical_fov";
static constexpr std::string_view PERSPECTIVE_NEAR         = "perspective_near";
static constexpr std::string_view PERSPECTIVE_FAR          = "perspective_far";
static constexpr std::string_view ORTHOGRAPHIC_SIZE        = "orthographic_size";
static constexpr std::string_view ORTHOGRAPHIC_NEAR        = "orthographic_near";
static constexpr std::string_view ORTHOGRAPHIC_FAR         = "orthographic_far";
static constexpr std::string_view PRIMARY                  = "primary";
static constexpr std::string_view FIXED_ASPECT_RATIO       = "fixed_aspect_ratio";

// component::Color
static constexpr std::string_view COLOR_COMPONENT = "color_component";
static constexpr std::string_view COLOR           = "color";

// component::SpriteRenderer
static constexpr std::string_view SPRITE_RENDERER_COMPONENT = "sprite_renderer_component";
static constexpr std::string_view TEXTURE_WIDTH             = "texture_width";
static constexpr std::string_view TEXTURE_HEIGHT            = "texture_height";
static constexpr std::string_view TEXTURE_PATH              = "texture_path";
static constexpr std::string_view TILING_FACTOR             = "tiling_factor";

// component::RigidBody2D
static constexpr std::string_view RIGID_BODY_2D_COMPONENT      = "rigid_body_2d_component";
static constexpr std::string_view RIGID_BODY_2D_TYPE           = "rigid_body_2d_type";
static constexpr std::string_view RIGID_BODY_2D_FIXED_ROTATION = "rigid_body_2d_fixed_rotation";

// component::BoxCollider2D
static constexpr std::string_view BOX_COLLIDER_2D_COMPONENT             = "box_collider_2d_component";
static constexpr std::string_view BOX_COLLIDER_2D_OFFSET                = "box_collider_2d_offset";
static constexpr std::string_view BOX_COLLIDER_2D_SIZE                  = "box_collider_2d_size";
static constexpr std::string_view BOX_COLLIDER_2D_DENSITY               = "box_collider_2d_density";
static constexpr std::string_view BOX_COLLIDER_2D_FRICTION              = "box_collider_2d_friction";
static constexpr std::string_view BOX_COLLIDER_2D_RESTITUTION           = "box_collider_2d_restitution";
static constexpr std::string_view BOX_COLLIDER_2D_RESTITUTION_THRESHOLD = "box_collider_2d_restitution_threshold";

}  // namespace serializer_keys

}  // namespace eclipse