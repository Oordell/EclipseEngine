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
static constexpr std::string_view TAG_TAG       = "tag";

// component::Transform
static constexpr std::string_view TRANSFORM_COMPONENT   = "transform_component";
static constexpr std::string_view TRANSFORM_TRANSLATION = "translation";
static constexpr std::string_view TRANSFORM_ROTATION    = "rotation";
static constexpr std::string_view TRANSFORM_SCALE       = "scale";

// component::Camera
static constexpr std::string_view CAMERA_COMPONENT                = "camera_component";
static constexpr std::string_view CAMERA_CAMERA                   = "camera";
static constexpr std::string_view CAMERA_PROJECTION_TYPE          = "projection_type";
static constexpr std::string_view CAMERA_PERSPECTIVE_VERTICAL_FOV = "perspective_vertical_fov";
static constexpr std::string_view CAMERA_PERSPECTIVE_NEAR         = "perspective_near";
static constexpr std::string_view CAMERA_PERSPECTIVE_FAR          = "perspective_far";
static constexpr std::string_view CAMERA_ORTHOGRAPHIC_SIZE        = "orthographic_size";
static constexpr std::string_view CAMERA_ORTHOGRAPHIC_NEAR        = "orthographic_near";
static constexpr std::string_view CAMERA_ORTHOGRAPHIC_FAR         = "orthographic_far";
static constexpr std::string_view CAMERA_PRIMARY                  = "primary";
static constexpr std::string_view CAMERA_FIXED_ASPECT_RATIO       = "fixed_aspect_ratio";

// component::Color
static constexpr std::string_view COLOR_COMPONENT = "color_component";
static constexpr std::string_view COLOR_COLOR     = "color";

// component::SpriteRenderer
static constexpr std::string_view SPRITE_RENDERER_COMPONENT      = "sprite_renderer_component";
static constexpr std::string_view SPRITE_RENDERER_TEXTURE_WIDTH  = "texture_width";
static constexpr std::string_view SPRITE_RENDERER_TEXTURE_HEIGHT = "texture_height";
static constexpr std::string_view SPRITE_RENDERER_TEXTURE_PATH   = "texture_path";
static constexpr std::string_view SPRITE_RENDERER_TILING_FACTOR  = "tiling_factor";
static constexpr std::string_view SPRITE_RENDERER_COLOR          = "color";

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

// component::CircleRenderer
static constexpr std::string_view CIRCLE_RENDERER_COMPONENT = "circle_renderer_component";
static constexpr std::string_view CIRCLE_RENDERER_COLOR     = "circle_renderer_color";
static constexpr std::string_view CIRCLE_RENDERER_RADIUS    = "circle_renderer_radius";
static constexpr std::string_view CIRCLE_RENDERER_THICKNESS = "circle_renderer_thickness";
static constexpr std::string_view CIRCLE_RENDERER_FADE      = "circle_renderer_fade";

// component::BoxCollider2D
static constexpr std::string_view CIRCLE_COLLIDER_2D_COMPONENT             = "circle_collider_2d_component";
static constexpr std::string_view CIRCLE_COLLIDER_2D_OFFSET                = "circle_collider_2d_offset";
static constexpr std::string_view CIRCLE_COLLIDER_2D_RADIUS                = "circle_collider_2d_radius";
static constexpr std::string_view CIRCLE_COLLIDER_2D_DENSITY               = "circle_collider_2d_density";
static constexpr std::string_view CIRCLE_COLLIDER_2D_FRICTION              = "circle_collider_2d_friction";
static constexpr std::string_view CIRCLE_COLLIDER_2D_RESTITUTION           = "circle_collider_2d_restitution";
static constexpr std::string_view CIRCLE_COLLIDER_2D_RESTITUTION_THRESHOLD = "circle_collider_2d_restitution_threshold";

// component::SubTexture
static constexpr std::string_view SUB_TEXTURE_COMPONENT    = "sub_texture_component";
static constexpr std::string_view SUB_TEXTURE_TEXTURE_PATH = "sub_texture_texture_path";
static constexpr std::string_view SUB_TEXTURE_TILE_INDEX_X = "sub_texture_tile_index_x";
static constexpr std::string_view SUB_TEXTURE_TILE_INDEX_Y = "sub_texture_tile_index_y";
static constexpr std::string_view SUB_TEXTURE_TILE_WIDTH   = "sub_texture_tile_width";
static constexpr std::string_view SUB_TEXTURE_TILE_HEIGHT  = "sub_texture_tile_height";

// component::TextureSheetComponent
static constexpr std::string_view TEXTURE_SHEET_COMPONENT          = "texture_sheet_component_component";
static constexpr std::string_view TEXTURE_SHEET_TEXTURE_PATH       = "texture_sheet_component_texture_path";
static constexpr std::string_view TEXTURE_SHEET_SUB_TILE_WIDTH     = "texture_sheet_component_sub_tile_width";
static constexpr std::string_view TEXTURE_SHEET_SUB_TILE_HEIGHT    = "texture_sheet_component_sub_tile_height";
static constexpr std::string_view TEXTURE_SHEET_SUB_TILE_SPACING_X = "texture_sheet_component_sub_tile_spacing_x";
static constexpr std::string_view TEXTURE_SHEET_SUB_TILE_SPACING_Y = "texture_sheet_component_sub_tile_spacing_y";

}  // namespace serializer_keys

}  // namespace eclipse