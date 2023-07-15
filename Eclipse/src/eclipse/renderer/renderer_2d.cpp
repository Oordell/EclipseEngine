#include "ecpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {

struct Renderer2DStorage {
	ref<VertexArray> quad_vertex_array;
	ref<Shader> texture_shader;
	ref<Texture2D> white_texture;
};

static scope<Renderer2DStorage> data;

void Renderer2D::init() {
	EC_PROFILE_FUNCTION();

	data                    = make_scope<Renderer2DStorage>();
	data->quad_vertex_array = VertexArray::create();

	float square_vertices[5 * 4] = {
	    /* clang-format off */
			-0.5F, -0.5F, 0.0F, 0.0F, 0.0F,
			0.5F, -0.5F, 0.0F, 1.0F, 0.0F,
			0.5F, 0.5F, 0.0F, 1.0F, 1.0F,
			-0.5F, 0.5F, 0.0F, 0.0F, 1.0F
	    /* clang-format on */
	};

	auto square_vertex_buffer = VertexBuffer::create(square_vertices, sizeof(square_vertices));
	square_vertex_buffer->set_layout({
	    {ShaderDataType::floatvec3, "position_"},
	    {ShaderDataType::floatvec2, "tex_coord_"},
	});
	data->quad_vertex_array->add_vertex_buffer(square_vertex_buffer);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	auto square_index_buffer_  = IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
	data->quad_vertex_array->set_index_buffer(square_index_buffer_);

	data->white_texture         = Texture2D::create(WindowSize {1, 1});
	uint32_t white_texture_data = 0xffffffff;
	data->white_texture->set_data(&white_texture_data, sizeof(white_texture_data));

	data->texture_shader = Shader::create(FilePath("assets/shaders/texture.glsl"));
	data->texture_shader->bind();
	data->texture_shader->set_int("u_texture", 0);
}

void Renderer2D::shutdown() {
	EC_PROFILE_FUNCTION();
	data.reset();
}

void Renderer2D::begin_scene(const OrthographicCamera& camera) {
	EC_PROFILE_FUNCTION();
	data->texture_shader->bind();
	data->texture_shader->set_mat4("view_projection", camera.get_view_projection_matrix());
}

void Renderer2D::end_scene() { EC_PROFILE_FUNCTION(); }

void Renderer2D::draw_quad(const QuadMetaDataPosition2D& info) {
	draw_quad(QuadMetaDataPosition3D {.position     = {info.position.x, info.position.y, 0.0F},
	                                  .rotation_deg = info.rotation_deg,
	                                  .size         = info.size,
	                                  .color        = info.color});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3D& info) {
	EC_PROFILE_FUNCTION();

	data->texture_shader->set_float4("u_color", info.color);
	data->white_texture->bind();

	glm::mat4 transform = compute_transform(info.position, info.rotation_deg, info.size);
	data->texture_shader->set_mat4("transform", transform);

	data->quad_vertex_array->bind();
	RenderCommand::draw_indexed(data->quad_vertex_array);
}

void Renderer2D::draw_quad(const QuadMetaDataPosition2DTexture& info) {
	draw_quad(QuadMetaDataPosition3DTexture {.position     = {info.position.x, info.position.y, 0.0F},
	                                         .rotation_deg = info.rotation_deg,
	                                         .size         = info.size,
	                                         .texture      = info.texture});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3DTexture& info) {
	EC_PROFILE_FUNCTION();
	data->texture_shader->set_float4("u_color", glm::vec4(1.0F));
	info.texture->bind();

	glm::mat4 transform = compute_transform(info.position, info.rotation_deg, info.size);
	data->texture_shader->set_mat4("transform", transform);

	data->quad_vertex_array->bind();
	RenderCommand::draw_indexed(data->quad_vertex_array);
}

glm::mat4 Renderer2D::compute_transform(const glm::vec3& position, float rotation_deg, const glm::vec2& size) {
	EC_PROFILE_FUNCTION();
	static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	static const glm::vec3 ROTATION_AXIS   = glm::vec3(0.0F, 0.0F, 1.0F);
	return glm::translate(IDENTITY_MATRIX, position) *
	       glm::rotate(IDENTITY_MATRIX, glm::radians(rotation_deg), ROTATION_AXIS) *
	       glm::scale(IDENTITY_MATRIX, {size.x, size.y, 1.0F});
}

}  // namespace eclipse