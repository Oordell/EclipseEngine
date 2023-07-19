#include "ecpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {

struct QuadVertex {
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 tex_coord;
};

struct Renderer2DData {
	static constexpr uint32_t MAX_QUADS                  = 10000;
	static constexpr uint32_t MAX_VERTICES               = MAX_QUADS * 4;
	static constexpr uint32_t MAX_INDICES                = MAX_QUADS * 6;
	static constexpr uint32_t QUAD_INDEX_COUNT_INCREMENT = 6;

	ref<VertexArray> quad_vertex_array;
	ref<VertexBuffer> quad_vertex_buffer;
	ref<Shader> texture_shader;
	ref<Texture2D> white_texture;

	uint32_t quad_index_count = 0;

	QuadVertex* quad_vertex_buffer_base = nullptr;
	QuadVertex* quad_vertex_buffer_ptr  = nullptr;
};

static Renderer2DData data;

void Renderer2D::init() {
	EC_PROFILE_FUNCTION();

	data.quad_vertex_array = VertexArray::create();

	data.quad_vertex_buffer = VertexBuffer::create(data.MAX_VERTICES * sizeof(QuadVertex));
	data.quad_vertex_buffer->set_layout({
	    {ShaderDataType::floatvec3, "position_"},
	    {ShaderDataType::floatvec4, "color_"},
	    {ShaderDataType::floatvec2, "tex_coord_"},
	});
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

	data.texture_shader = Shader::create(FilePath("assets/shaders/texture.glsl"));
	data.texture_shader->bind();
	data.texture_shader->set_int("u_texture", 0);
}

void Renderer2D::shutdown() { EC_PROFILE_FUNCTION(); }

void Renderer2D::begin_scene(const OrthographicCamera& camera) {
	EC_PROFILE_FUNCTION();
	data.texture_shader->bind();
	data.texture_shader->set_mat4("view_projection", camera.get_view_projection_matrix());

	data.quad_index_count       = 0;
	data.quad_vertex_buffer_ptr = data.quad_vertex_buffer_base;
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

	/*data.texture_shader->set_float4("u_color", info.color);
	data.texture_shader->set_float("u_tiling_factor", info.tiling_factor);
	data.white_texture->bind();

	glm::mat4 transform = compute_transform(info.position, info.rotation_deg, info.size);
	data.texture_shader->set_mat4("transform", transform);

	data.quad_vertex_array->bind();
	RenderCommand::draw_indexed(data.quad_vertex_array);*/
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

	/*data.texture_shader->set_float4("u_color", info.tint_color);
	data.texture_shader->set_float("u_tiling_factor", info.tiling_factor);
	info.texture->bind();

	glm::mat4 transform = compute_transform(info.position, info.rotation_deg, info.size);
	data.texture_shader->set_mat4("transform", transform);

	data.quad_vertex_array->bind();
	RenderCommand::draw_indexed(data.quad_vertex_array);*/
}

void Renderer2D::draw_quad_impl(const QuadDrawingDataImpl& info) {
	EC_PROFILE_FUNCTION();

	data.quad_vertex_buffer_ptr->position  = info.position;
	data.quad_vertex_buffer_ptr->color     = info.tint_color;
	data.quad_vertex_buffer_ptr->tex_coord = {0.0F, 0.0F};
	data.quad_vertex_buffer_ptr++;

	data.quad_vertex_buffer_ptr->position  = {info.position.x + info.size.x, info.position.y, 0.0F};
	data.quad_vertex_buffer_ptr->color     = info.tint_color;
	data.quad_vertex_buffer_ptr->tex_coord = {1.0F, 0.0F};
	data.quad_vertex_buffer_ptr++;

	data.quad_vertex_buffer_ptr->position  = {info.position.x + info.size.x, info.position.y + info.size.y, 0.0F};
	data.quad_vertex_buffer_ptr->color     = info.tint_color;
	data.quad_vertex_buffer_ptr->tex_coord = {1.0F, 1.0F};
	data.quad_vertex_buffer_ptr++;

	data.quad_vertex_buffer_ptr->position  = {info.position.x, info.position.y + info.size.y, 0.0F};
	data.quad_vertex_buffer_ptr->color     = info.tint_color;
	data.quad_vertex_buffer_ptr->tex_coord = {0.0F, 1.0F};
	data.quad_vertex_buffer_ptr++;

	data.quad_index_count += data.QUAD_INDEX_COUNT_INCREMENT;

	/*data.texture_shader->set_float4("u_color", info.tint_color);
	data.texture_shader->set_float("u_tiling_factor", info.tiling_factor);
	info.texture->bind();

	glm::mat4 transform = compute_transform(info.position, info.rotation_deg, info.size);
	data.texture_shader->set_mat4("transform", transform);

	data.quad_vertex_array->bind();
	RenderCommand::draw_indexed(data.quad_vertex_array);*/
}  // namespace eclipse

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