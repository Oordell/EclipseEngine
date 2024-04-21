#include "ecpch.h"
#include "renderer_2d.h"
#include "eclipse/renderer/uniform_buffer.h"

#include "vertex_array.h"
#include "shader.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

struct QuadVertex {
	glm::vec3 position {0.0F, 0.0F, 0.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	glm::vec2 tex_coord {0.0F, 0.0F};
	float texture_index {0.0F};
	float tiling_factor {1.0F};

	// Editor only:
	int entity_id {-1};
};

struct Renderer2DData {
	static constexpr uint32_t MAX_QUADS                  = 10000;
	static constexpr uint32_t MAX_VERTICES               = MAX_QUADS * 4;
	static constexpr uint32_t MAX_INDICES                = MAX_QUADS * 6;
	static constexpr uint32_t QUAD_INDEX_COUNT_INCREMENT = 6;
	/* TODO: Implement renderer capabilities, to query render data from the GPU. This is hard-coded for now: */
	static constexpr uint32_t MAX_TEXTURE_SLOTS = 32;

	ref<VertexArray> quad_vertex_array;
	ref<VertexBuffer> quad_vertex_buffer;
	ref<Shader> texture_shader;
	ref<Texture2D> white_texture;

	uint32_t quad_index_count = 0;

	QuadVertex* quad_vertex_buffer_base = nullptr;
	QuadVertex* quad_vertex_buffer_ptr  = nullptr;

	std::array<ref<Texture2D>, MAX_TEXTURE_SLOTS> texture_slots;
	uint32_t texture_slot_index = 1;  // index 0 = white texture.

	glm::vec4 quad_vertex_positions[defaults::NUM_OF_QUAD_CORNERS];

	RendererStatistics stats;

	struct CameraData {
		glm::mat4 view_projection {};
	};

	CameraData camera_buffer;
	ref<UniformBuffer> camera_uniform_buffer;
};

static Renderer2DData data;

void Renderer2D::init() {
	EC_PROFILE_FUNCTION();

	data.quad_vertex_array = VertexArray::create();

	data.quad_vertex_buffer = VertexBuffer::create(data.MAX_VERTICES * sizeof(QuadVertex));
	data.quad_vertex_buffer->set_layout({{ShaderDataType::floatvec3, "position_"},
	                                     {ShaderDataType::floatvec4, "color_"},
	                                     {ShaderDataType::floatvec2, "tex_coord_"},
	                                     {ShaderDataType::floatvec1, "tex_index_"},
	                                     {ShaderDataType::floatvec1, "tiling_factor_"},
	                                     {ShaderDataType::intvec1, "entity_id"}});
	data.quad_vertex_array->add_vertex_buffer(data.quad_vertex_buffer);

	data.quad_vertex_buffer_base = new QuadVertex[data.MAX_VERTICES];

	uint32_t* quad_indices = new uint32_t[data.MAX_INDICES];

	uint32_t offset = 0;
	for (uint32_t i = 0; i < data.MAX_INDICES; i += data.QUAD_INDEX_COUNT_INCREMENT) {
		quad_indices[i + 0] = offset + 0;
		quad_indices[i + 1] = offset + 1;
		quad_indices[i + 2] = offset + 2;

		quad_indices[i + 3] = offset + 2;
		quad_indices[i + 4] = offset + 3;
		quad_indices[i + 5] = offset + 0;

		offset += 4;
	}

	auto quad_index_buffer_ = IndexBuffer::create(quad_indices, data.MAX_INDICES);
	data.quad_vertex_array->set_index_buffer(quad_index_buffer_);
	delete[] quad_indices;

	data.white_texture          = Texture2D::create(WindowSize {units::pixels(1), units::pixels(1)});
	uint32_t white_texture_data = 0xffffffff;
	data.white_texture->set_data(&white_texture_data, sizeof(white_texture_data));

	int32_t samplers[data.MAX_TEXTURE_SLOTS];
	for (int32_t i = 0; i < data.MAX_TEXTURE_SLOTS; i++) {
		samplers[i] = i;
	}

	data.texture_shader = Shader::create(FilePath("assets/shaders/texture.glsl"));

	data.texture_slots[0] = data.white_texture;

	data.quad_vertex_positions[0] = {-0.5F, -0.5F, 0.0F, 1.0F};
	data.quad_vertex_positions[1] = {0.5F, -0.5F, 0.0F, 1.0F};
	data.quad_vertex_positions[2] = {0.5F, 0.5F, 0.0F, 1.0F};
	data.quad_vertex_positions[3] = {-0.5F, 0.5F, 0.0F, 1.0F};

	data.camera_uniform_buffer = UniformBuffer::create({.size = sizeof(Renderer2DData::CameraData), .binding = 0});
}

void Renderer2D::shutdown() {
	EC_PROFILE_FUNCTION();
	delete[] data.quad_vertex_buffer_base;
}

void Renderer2D::begin_scene(const RenderCamera& camera) {
	EC_PROFILE_FUNCTION();
	init_scene(camera.projection * glm::inverse(camera.transform));
}

void Renderer2D::begin_scene(const EditorCamera& camera) {
	EC_PROFILE_FUNCTION();
	init_scene(camera.get_view_projection());
}

void Renderer2D::begin_scene(const OrthographicCamera& camera) {
	EC_PROFILE_FUNCTION();
	init_scene(camera.get_view_projection_matrix());
}

void Renderer2D::end_scene() {
	EC_PROFILE_FUNCTION();

	uint32_t data_size = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(data.quad_vertex_buffer_ptr) -
	                                           reinterpret_cast<uint8_t*>(data.quad_vertex_buffer_base));
	data.quad_vertex_buffer->set_data(data.quad_vertex_buffer_base, data_size);

	flush();
}

void Renderer2D::flush() {
	EC_PROFILE_FUNCTION();

	if (data.quad_index_count == 0) {
		return;  // Nothing to draw
	}

	for (uint32_t i = 0; i < data.texture_slot_index; i++) {
		data.texture_slots[i]->bind(i);
	}
	data.texture_shader->bind();
	RenderCommand::draw_indexed(data.quad_vertex_array, data.quad_index_count);
	data.stats.draw_calls++;
}

void Renderer2D::draw_quad(const QuadMetaDataPosition2D& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture   = data.white_texture,
	                .transform = compute_transform({info.spatial_info.position.x, info.spatial_info.position.y, 0.0F},
	                                               info.spatial_info.rotation_rad, info.spatial_info.size),
	                .common    = info.common});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3D& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture   = data.white_texture,
	                .transform = compute_transform(info.spatial_info.position, info.spatial_info.rotation_rad,
	                                               info.spatial_info.size),
	                .common    = info.common});
}

void Renderer2D::draw_quad(const QuadMetaDataTransform& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture = data.white_texture, .transform = info.transform, .common = info.common});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition2DTexture& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture   = info.texture,
	                .transform = compute_transform({info.spatial_info.position.x, info.spatial_info.position.y, 0.0F},
	                                               info.spatial_info.rotation_rad, info.spatial_info.size),
	                .common    = info.common});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3DTexture& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture   = info.texture,
	                .transform = compute_transform(info.spatial_info.position, info.spatial_info.rotation_rad,
	                                               info.spatial_info.size),
	                .common    = info.common});
}

void Renderer2D::draw_quad(const QuadMetaDataTransformTexture& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture = info.texture, .transform = info.transform, .common = info.common});
}

void Renderer2D::draw_sprite(const SpriteMetaDataTransform& info) {
	EC_PROFILE_FUNCTION();
	draw_quad_impl({.texture   = info.component.texture ? info.component.texture : data.white_texture,
	                .transform = info.transform,
	                .common    = {.tiling_factor  = info.component.tiling_factor,
	                              .color          = info.component.color,
	                              .texture_coords = info.common.texture_coords},
	                .entity_id = info.entity_id});
}

void Renderer2D::draw_quad_impl(const QuadDrawingDataImpl& info) {
	EC_PROFILE_FUNCTION();

	if (info.texture == nullptr) {
		return;
	}

	if (data.quad_index_count >= Renderer2DData::MAX_INDICES ||
	    data.texture_slot_index >= Renderer2DData::MAX_TEXTURE_SLOTS) {
		end_scene_and_start_new_batch();
	}

	float texture_index = -1.0F;
	{
		EC_PROFILE_SCOPE("Finding texture index");
		for (uint32_t i = 0; i < data.texture_slot_index; i++) {
			if (data.texture_slots[i]->get_renderer_id() == info.texture->get_renderer_id()) {
				texture_index = static_cast<float>(i);
				break;
			}
		}

		if (texture_index == -1.0F) {
			texture_index                               = static_cast<float>(data.texture_slot_index);
			data.texture_slots[data.texture_slot_index] = info.texture;
			data.texture_slot_index++;
		}
	}

	{
		EC_PROFILE_SCOPE("Adding data to quad vertex buffer ptr.");
		for (int32_t i = 0; i < defaults::NUM_OF_QUAD_CORNERS; i++) {
			data.quad_vertex_buffer_ptr->position      = info.transform * data.quad_vertex_positions[i];
			data.quad_vertex_buffer_ptr->color         = info.common.color;
			data.quad_vertex_buffer_ptr->tex_coord     = info.common.texture_coords[i];
			data.quad_vertex_buffer_ptr->texture_index = texture_index;
			data.quad_vertex_buffer_ptr->tiling_factor = info.common.tiling_factor;
			data.quad_vertex_buffer_ptr->entity_id     = info.entity_id;
			data.quad_vertex_buffer_ptr++;
		}
	}

	data.quad_index_count += data.QUAD_INDEX_COUNT_INCREMENT;
	data.stats.quad_count++;
}

glm::mat4 Renderer2D::compute_transform(const glm::vec3& position, float rotation_rad, const glm::vec2& size) {
	EC_PROFILE_FUNCTION();
	static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	static const glm::vec3 ROTATION_AXIS   = glm::vec3(0.0F, 0.0F, 1.0F);
	static const float ROTATION_THRESHOLD  = 0.001F;
	if (std::abs(rotation_rad) > ROTATION_THRESHOLD) {
		return glm::translate(IDENTITY_MATRIX, position) * glm::rotate(IDENTITY_MATRIX, rotation_rad, ROTATION_AXIS) *
		       glm::scale(IDENTITY_MATRIX, {size.x, size.y, 1.0F});
	}
	return glm::translate(IDENTITY_MATRIX, position) * glm::scale(IDENTITY_MATRIX, {size.x, size.y, 1.0F});
}

void Renderer2D::end_scene_and_start_new_batch() {
	end_scene();
	reset_data();
}

void Renderer2D::init_scene(const glm::mat4& view_projection) {
	EC_PROFILE_FUNCTION();

	data.camera_buffer.view_projection = view_projection;
	data.camera_uniform_buffer->set_data(&data.camera_buffer, {.size = sizeof(Renderer2DData::CameraData), .offset = 0});
	reset_data();
}

void Renderer2D::reset_data() {
	data.quad_index_count       = 0;
	data.quad_vertex_buffer_ptr = data.quad_vertex_buffer_base;
	data.texture_slot_index     = 1;
}

void Renderer2D::reset_statistics() { memset(&data.stats, 0, sizeof(RendererStatistics)); }

RendererStatistics Renderer2D::get_statistics() { return data.stats; }

}  // namespace eclipse