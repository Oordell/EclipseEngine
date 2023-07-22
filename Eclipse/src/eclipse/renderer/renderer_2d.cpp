#include "ecpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {

struct QuadVertex {
	glm::vec3 position {0.0F, 0.0F, 0.0F};
	glm::vec4 color {1.0F, 1.0F, 1.0F, 1.0F};
	glm::vec2 tex_coord {0.0F, 0.0F};
	float texture_index {0.0F};
	float tiling_factor {1.0F};
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
	                                     {ShaderDataType::floatvec1, "tiling_factor_"}});
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

	data.white_texture          = Texture2D::create(WindowSize {1, 1});
	uint32_t white_texture_data = 0xffffffff;
	data.white_texture->set_data(&white_texture_data, sizeof(white_texture_data));

	int32_t samplers[data.MAX_TEXTURE_SLOTS];
	for (int32_t i = 0; i < data.MAX_TEXTURE_SLOTS; i++) {
		samplers[i] = i;
	}

	data.texture_shader = Shader::create(FilePath("assets/shaders/texture.glsl"));
	data.texture_shader->bind();
	data.texture_shader->set_int_array("u_textures", samplers, data.MAX_TEXTURE_SLOTS);

	data.texture_slots[0] = data.white_texture;
}

void Renderer2D::shutdown() { EC_PROFILE_FUNCTION(); }

void Renderer2D::begin_scene(const OrthographicCamera& camera) {
	EC_PROFILE_FUNCTION();
	data.texture_shader->bind();
	data.texture_shader->set_mat4("view_projection", camera.get_view_projection_matrix());

	data.quad_index_count       = 0;
	data.quad_vertex_buffer_ptr = data.quad_vertex_buffer_base;
	data.texture_slot_index     = 1;
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

	for (uint32_t i = 0; i < data.texture_slot_index; i++) {
		data.texture_slots[i]->bind(i);
	}
	RenderCommand::draw_indexed(data.quad_vertex_array, data.quad_index_count);
}

void Renderer2D::draw_quad(const QuadMetaDataPosition2D& info) {
	draw_quad(QuadMetaDataPosition3D {.position      = {info.position.x, info.position.y, 0.0F},
	                                  .rotation_deg  = info.rotation_deg,
	                                  .size          = info.size,
	                                  .tiling_factor = info.tiling_factor,
	                                  .color         = info.color});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3D& info) {
	EC_PROFILE_FUNCTION();

	draw_quad_impl({.position      = info.position,
	                .rotation_deg  = info.rotation_deg,
	                .size          = info.size,
	                .tiling_factor = info.tiling_factor,
	                .texture       = data.white_texture,
	                .tint_color    = info.color});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition2DTexture& info) {
	draw_quad(QuadMetaDataPosition3DTexture {.position      = {info.position.x, info.position.y, 0.0F},
	                                         .rotation_deg  = info.rotation_deg,
	                                         .size          = info.size,
	                                         .tiling_factor = info.tiling_factor,
	                                         .texture       = info.texture,
	                                         .tint_color    = info.tint_color});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3DTexture& info) {
	EC_PROFILE_FUNCTION();

	draw_quad_impl(info);
}

void Renderer2D::draw_quad_impl(const QuadDrawingDataImpl& info) {
	EC_PROFILE_FUNCTION();

	float texture_index = -1.0F;
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

	static const int32_t CORNERS                    = 4;
	const std::array<glm::vec2, CORNERS> sizes      = {glm::vec2 {0.0F, 0.0F}, glm::vec2 {info.size.x, 0.0F},
	                                                   glm::vec2 {info.size.x, info.size.y}, glm::vec2 {0.0F, info.size.y}};
	const std::array<glm::vec2, CORNERS> tex_coords = {glm::vec2 {0.0F, 0.0F}, glm::vec2 {1.0F, 0.0F},
	                                                   glm::vec2 {1.0F, 1.0F}, glm::vec2 {0.0F, 1.0F}};
	for (int32_t i = 0; i < CORNERS; i++) {
		map_quad_vertex_buffer_info({.position      = info.position,
		                             .size          = sizes[i],
		                             .color         = info.tint_color,
		                             .tex_coord     = tex_coords[i],
		                             .texture_index = texture_index,
		                             .tiling_factor = info.tiling_factor});
	}

	data.quad_index_count += data.QUAD_INDEX_COUNT_INCREMENT;
}

void Renderer2D::map_quad_vertex_buffer_info(const QuadVertexBufferMappingInfo& info) {
	data.quad_vertex_buffer_ptr->position      = {info.position.x + info.size.x, info.position.y + info.size.y,
	                                              info.position.z};
	data.quad_vertex_buffer_ptr->color         = info.color;
	data.quad_vertex_buffer_ptr->tex_coord     = info.tex_coord;
	data.quad_vertex_buffer_ptr->texture_index = info.texture_index;
	data.quad_vertex_buffer_ptr->tiling_factor = info.tiling_factor;
	data.quad_vertex_buffer_ptr++;
}

glm::mat4 Renderer2D::compute_transform(const glm::vec3& position, float rotation_deg, const glm::vec2& size) {
	EC_PROFILE_FUNCTION();
	static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	static const glm::vec3 ROTATION_AXIS   = glm::vec3(0.0F, 0.0F, 1.0F);
	static const float rotation_threshold  = 0.01F;
	if (std::abs(rotation_deg) > rotation_threshold) {
		return glm::translate(IDENTITY_MATRIX, position) *
		       glm::rotate(IDENTITY_MATRIX, glm::radians(rotation_deg), ROTATION_AXIS) *
		       glm::scale(IDENTITY_MATRIX, {size.x, size.y, 1.0F});
	}
	return glm::translate(IDENTITY_MATRIX, position) * glm::scale(IDENTITY_MATRIX, {size.x, size.y, 1.0F});
}

}  // namespace eclipse