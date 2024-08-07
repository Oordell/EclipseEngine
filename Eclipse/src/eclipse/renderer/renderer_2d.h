#pragma once

#include "eclipse/core/core.h"
#include "orthographic_camera.h"
#include "eclipse/renderer/camera.h"
#include "eclipse/renderer/editor_camera.h"
#include "eclipse/renderer/texture.h"
#include "eclipse/common_types/must_init.h"
#include "eclipse/scene/components.h"
#include "eclipse/scene/texture_sheets_components.h"

#include <au.hh>

namespace eclipse {

namespace defaults {

static constexpr int32_t NUM_OF_QUAD_CORNERS                           = 4;
static constexpr glm::vec2 DEFAULT_TEXTURE_COORDS[NUM_OF_QUAD_CORNERS] = {
    glm::vec2 {0.0F, 0.0F}, glm::vec2 {1.0F, 0.0F}, glm::vec2 {1.0F, 1.0F}, glm::vec2 {0.0F, 1.0F}};

}  // namespace defaults

struct QuadMetaDataCommon {
	float tiling_factor {1.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	const glm::vec2* texture_coords = defaults::DEFAULT_TEXTURE_COORDS;
};

struct QuadSpatialInfo2D {
	glm::vec2 position {0.0F, 0.0F};
	float rotation_rad {0.0F};
	glm::vec2 size {0.0F, 0.0F};
};

struct QuadSpatialInfo3D {
	glm::vec3 position {0.0F, 0.0F, 0.0F};
	float rotation_rad {0.0F};
	glm::vec2 size {0.0F, 0.0F};
};

struct QuadMetaDataPosition2D {
	QuadSpatialInfo2D spatial_info {};
	QuadMetaDataCommon common {};
};

struct QuadMetaDataPosition3D {
	QuadSpatialInfo3D spatial_info {};
	QuadMetaDataCommon common {};
};

struct QuadMetaDataTransform {
	glm::mat4 transform {1.0F};
	QuadMetaDataCommon common {};
};

struct QuadMetaDataPosition2DTexture {
	QuadSpatialInfo2D spatial_info {};
	QuadMetaDataCommon common {};
	MustInit<ref<Texture2D>> texture;
};

struct QuadMetaDataPosition3DTexture {
	QuadSpatialInfo3D spatial_info {};
	QuadMetaDataCommon common {};
	MustInit<ref<Texture2D>> texture;
};

struct QuadMetaDataTransformTexture {
	glm::mat4 transform {1.0F};
	QuadMetaDataCommon common {};
	MustInit<ref<Texture2D>> texture;
};

struct CircleMetaData {
	glm::mat4 transform {1.0F};
	component::CircleRenderer component;
	int entity_id {-1};
};

struct LineMetaData {
	glm::vec3 start {0.0F, 0.0F, 0.0F};
	glm::vec3 end {0.0F, 0.0F, 0.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	int entity_id {-1};
};

struct LineLoopMetaData {
	std::vector<glm::vec3> line_vertices;
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	int entity_id {-1};
};

struct RectangleMetaDataPosition {
	glm::vec3 position {0.0F, 0.0F, 0.0F};
	glm::vec2 size {0.0F, 0.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	int entity_id {-1};
};

struct RectangleMetaDataTransform {
	glm::mat4 transform {1.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	int entity_id {-1};
};

struct SpriteMetaDataTransform {
	QuadMetaDataCommon common {};
	glm::mat4 transform {1.0F};
	component::SpriteRenderer component {};
	int entity_id {-1};
};

struct SubTextureMetaDataTransform {
	QuadMetaDataCommon common {};
	glm::mat4 transform {1.0F};
	component::SubTexture component {};
	int entity_id {-1};
};

struct QuadDrawingDataImpl {
	ref<Texture2D> texture;
	glm::mat4 transform {1.0F};
	QuadMetaDataCommon common {};
	int entity_id {-1};
};

struct RendererStatistics {
	uint32_t draw_calls = 0;
	uint32_t quad_count = 0;

	uint32_t get_total_vertex_count() { return quad_count * 4; }

	uint32_t get_total_index_count() { return quad_count * 6; }
};

struct RenderCamera {
	glm::mat4 projection {1.0F};
	glm::mat4 transform {1.0F};
};

class ECLIPSE_API Renderer2D {
public:
	static void init();
	static void shutdown();
	static void begin_scene(const RenderCamera& camera);
	static void begin_scene(const EditorCamera& camera);
	static void begin_scene(const OrthographicCamera& camera);
	static void end_scene();
	static void flush();

	static void draw_quad(const QuadMetaDataPosition2D& info);
	static void draw_quad(const QuadMetaDataPosition3D& info);
	static void draw_quad(const QuadMetaDataTransform& info);
	static void draw_quad(const QuadMetaDataPosition2DTexture& info);
	static void draw_quad(const QuadMetaDataPosition3DTexture& info);
	static void draw_quad(const QuadMetaDataTransformTexture& info);

	static void draw_circle(const CircleMetaData& info);
	static void draw_line(const LineMetaData& info);
	static void draw_line_loop(const LineLoopMetaData& info);

	static void draw_rectangle(const RectangleMetaDataPosition& info);
	static void draw_rectangle(const RectangleMetaDataTransform& info);

	static void draw_sprite(const SpriteMetaDataTransform& info);
	static void draw_sub_texture(const SubTextureMetaDataTransform& info);

	static au::QuantityF<units::Pixels> get_line_width();
	static void set_line_width(au::QuantityF<units::Pixels> width);

private:
	static void draw_quad_impl(const QuadDrawingDataImpl& info);
	static glm::mat4 compute_transform(const glm::vec3& position, float rotation_rad, const glm::vec2& size);
	static void end_scene_and_start_new_batch();
	static void init_scene(const glm::mat4& view_projection);
	static void reset_data();

	static void flush_quads();
	static void flush_circles();
	static void flush_lines();

public:
	static void reset_statistics();
	static RendererStatistics get_statistics();
};

}  // namespace eclipse