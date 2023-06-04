#include "ecpch.h"
#include "renderer.h"

#include "platform/opengl/opengl_shader.h"

namespace eclipse {

Renderer::SceneData* Renderer::scene_data_ = new Renderer::SceneData;

void Renderer::begin_scene(OrthographicCamera& camera) {
	scene_data_->view_projection_matrix = camera.get_view_projection_matrix();
}

void Renderer::end_scene() {}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array,
                      const glm::mat4& transform) {
	shader->bind();
	std::dynamic_pointer_cast<OpenGLShader>(shader)->upload_uniform_mat4("view_projection",
	                                                                     scene_data_->view_projection_matrix);
	std::dynamic_pointer_cast<OpenGLShader>(shader)->upload_uniform_mat4("transform", transform);

	vertex_array->bind();
	RenderCommand::draw_indexed(vertex_array);
}

}  // namespace eclipse