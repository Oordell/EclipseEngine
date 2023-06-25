#include "ecpch.h"
#include "renderer_2d.h"

#include "vertex_array.h"
#include "shader.h"
#include "render_command.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {

struct Renderer2DStorage {
	ref<VertexArray> quad_vertex_array;
	ref<Shader> flat_color_shader;
};

static scope<Renderer2DStorage> data;

void Renderer2D::init() {
	data                    = make_scope<Renderer2DStorage>();
	data->quad_vertex_array = VertexArray::create();

	float square_vertices[3 * 4] = {
	    /* clang-format off */
			-0.5F, -0.5F, 0.0F,
			0.5F, -0.5F, 0.0F,
			0.5F, 0.5F, 0.0F,
			-0.5F, 0.5F, 0.0F
	    /* clang-format on */
	};

	auto square_vertex_buffer = VertexBuffer::create(square_vertices, sizeof(square_vertices));
	square_vertex_buffer->set_layout({{ShaderDataType::floatvec3, "position_"}});
	data->quad_vertex_array->add_vertex_buffer(square_vertex_buffer);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	auto square_index_buffer_  = IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
	data->quad_vertex_array->set_index_buffer(square_index_buffer_);

	data->flat_color_shader = Shader::create(FilePath("assets/shaders/flat_color.glsl"));
}

void Renderer2D::shutdown() { data.reset(); }

void Renderer2D::begin_scene(const OrthographicCamera& camera) {
	data->flat_color_shader->bind();
	data->flat_color_shader->set_mat4("view_projection", camera.get_view_projection_matrix());
}

void Renderer2D::end_scene() {}

void Renderer2D::draw_quad(const QuadMetaDataPosition2D& info) {
	draw_quad(QuadMetaDataPosition3D {.position     = {info.position.x, info.position.y, 0.0F},
	                                  .rotation_deg = info.rotation_deg,
	                                  .size         = info.size,
	                                  .color        = info.color});
}

void Renderer2D::draw_quad(const QuadMetaDataPosition3D& info) {
	data->flat_color_shader->bind();
	data->flat_color_shader->set_float4("u_color", info.color);

	static const glm::mat4 IDENTITY_MATRIX = glm::mat4(1.0F);
	static const glm::vec3 ROTATION_AXIS   = glm::vec3(0.0F, 0.0F, 1.0F);
	glm::mat4 transform                    = glm::translate(IDENTITY_MATRIX, info.position) *
	                      glm::rotate(IDENTITY_MATRIX, glm::radians(info.rotation_deg), ROTATION_AXIS) *
	                      glm::scale(IDENTITY_MATRIX, {info.size.x, info.size.y, 1.0F});

	data->flat_color_shader->set_mat4("transform", transform);

	data->quad_vertex_array->bind();
	RenderCommand::draw_indexed(data->quad_vertex_array);
}

}  // namespace eclipse