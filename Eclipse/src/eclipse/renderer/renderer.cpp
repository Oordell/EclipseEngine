#include "ecpch.h"
#include "renderer.h"

namespace eclipse {

Renderer::SceneData* Renderer::scene_data_ = new Renderer::SceneData;

void Renderer::begin_scene(OrthographicCamera& camera) {
	scene_data_->view_projection_matrix = camera.get_view_projection_matrix();
}

void Renderer::end_scene() {}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array) {
	shader->bind();
	shader->upload_uniform_mat4("view_projection", scene_data_->view_projection_matrix);

	vertex_array->bind();
	RenderCommand::draw_indexed(vertex_array);
}

}  // namespace eclipse