#include "ecpch.h"
#include "renderer.h"
#include "eclipse/renderer/renderer_2d.h"

#include "platform/opengl/opengl_shader.h"

namespace eclipse {

scope<Renderer::SceneData> Renderer::scene_data_ = make_scope<Renderer::SceneData>();

void Renderer::init() { 
	RenderCommand::init(); 
	Renderer2D::init();
}

void Renderer::on_window_resize(const WindowSize& window_size) {
	RenderCommand::set_viewport({.x = 0, .y = 0}, window_size);
}

void Renderer::begin_scene(OrthographicCamera& camera) {
	scene_data_->view_projection_matrix = camera.get_view_projection_matrix();
}

void Renderer::end_scene() {}

void Renderer::submit(const ref<Shader>& shader, const ref<VertexArray>& vertex_array, const glm::mat4& transform) {
	shader->bind();
	std::dynamic_pointer_cast<OpenGLShader>(shader)->upload_uniform_mat4("view_projection",
	                                                                     scene_data_->view_projection_matrix);
	std::dynamic_pointer_cast<OpenGLShader>(shader)->upload_uniform_mat4("transform", transform);

	vertex_array->bind();
	RenderCommand::draw_indexed(vertex_array);
}

}  // namespace eclipse