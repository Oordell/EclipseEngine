#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sandbox2D::on_attach() {
	float square_vertices[3 * 4] = {
	    /* clang-format off */
			-0.5F, -0.5F, 0.0F,
			0.5F, -0.5F, 0.0F,
			0.5F, 0.5F, 0.0F,
			-0.5F, 0.5F, 0.0F
	    /* clang-format on */
	};

	auto square_vertex_buffer = eclipse::VertexBuffer::create(square_vertices, sizeof(square_vertices));
	square_vertex_buffer->set_layout({{eclipse::ShaderDataType::floatvec3, "position_"}});
	square_vertex_array_->add_vertex_buffer(square_vertex_buffer);

	uint32_t square_indices[6] = {0, 1, 2, 2, 3, 0};
	auto square_index_buffer_  = eclipse::IndexBuffer::create(square_indices, sizeof(square_indices) / sizeof(uint32_t));
	square_vertex_array_->set_index_buffer(square_index_buffer_);
}

void Sandbox2D::on_detach() {}

void Sandbox2D::on_update(eclipse::Timestep timestep) {
	camera_controller_.on_update(timestep);

	frame_rate_ = static_cast<unsigned int>(1.0F / timestep);
	EC_TRACE_THROTTLED(0.25, "Frame rate: {0}Hz", frame_rate_);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
	eclipse::RenderCommand::clear();

	eclipse::Renderer::begin_scene(camera_controller_.get_camera());

	std::dynamic_pointer_cast<eclipse::OpenGLShader>(flat_color_shader_)->bind();
	std::dynamic_pointer_cast<eclipse::OpenGLShader>(flat_color_shader_)->upload_uniform_float4("u_color", square_color_);

	static const glm::mat4 identity_matrix = glm::mat4(1.0F);
	eclipse::Renderer::submit(flat_color_shader_, square_vertex_array_, glm::scale(identity_matrix, glm::vec3(1.5F)));

	eclipse::Renderer::end_scene();
}

void Sandbox2D::on_event(eclipse::Event& event) { camera_controller_.on_event(event); }

void Sandbox2D::on_imgui_render() {
	std::string text = "Frame rate: " + std::to_string(frame_rate_);
	ImGui::Begin("Framerate");
	ImGui::Text(text.c_str());
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square color", glm::value_ptr(square_color_));
	ImGui::End();
}
