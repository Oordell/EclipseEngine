#pragma once

#include "eclipse/core/core.h"
#include "orthographic_camera.h"
#include "eclipse/renderer/texture.h"
#include "eclipse/common_types/must_init.h"

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

struct QuadDrawingDataImpl {
	ref<Texture2D> texture;
	glm::mat4 transform {1.0F};
	float tiling_factor {1.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	const glm::vec2* texture_coords = defaults::DEFAULT_TEXTURE_COORDS;
};

struct RendererStatistics {
	uint32_t draw_calls = 0;
	uint32_t quad_count = 0;

	uint32_t get_total_vertex_count() { return quad_count * 4; }

	uint32_t get_total_index_count() { return quad_count * 6; }
};

class ECLIPSE_API Renderer2D {
public:
	static void init();
	static void shutdown();
	static void begin_scene(const OrthographicCamera& camera);
	static void end_scene();
	static void flush();

	static void draw_quad(const QuadMetaDataPosition2D& info);
	static void draw_quad(const QuadMetaDataPosition3D& info);
	static void draw_quad(const QuadMetaDataTransform& info);
	static void draw_quad(const QuadMetaDataPosition2DTexture& info);
	static void draw_quad(const QuadMetaDataPosition3DTexture& info);
	static void draw_quad(const QuadMetaDataTransformTexture& info);

private:
	static void draw_quad_impl(const QuadDrawingDataImpl& info);
	static glm::mat4 compute_transform(const glm::vec3& position, float rotation_rad, const glm::vec2& size);
	static void end_scene_and_start_new_batch();

public:
	static void reset_statistics();
	static RendererStatistics get_statistics();
};

}  // namespace eclipse