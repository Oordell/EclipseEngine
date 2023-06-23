#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sandbox2D::on_attach() {}

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

	eclipse::Renderer2D::begin_scene(camera_controller_.get_camera());
	eclipse::Renderer2D::draw_quad(
	    eclipse::QuadMetaDataPosition2D {.size = {1.0F, 1.0F}, .color = {0.8F, 0.2F, 0.3F, 1.0F}});
	eclipse::Renderer2D::end_scene();

	/*
	std::dynamic_pointer_cast<eclipse::OpenGLShader>(flat_color_shader_)->bind();
	std::dynamic_pointer_cast<eclipse::OpenGLShader>(flat_color_shader_)->upload_uniform_float4("u_color", square_color_);
	*/
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
