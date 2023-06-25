#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sandbox2D::on_attach() { checkerboard_texture_ = eclipse::Texture2D::create("assets/textures/Checkerboard.png"); }

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
	eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition2D {
	    .position = {-1.0F, 0.0F}, .rotation_deg = 0.0F, .size = {0.8F, 0.8F}, .color = {0.8F, 0.2F, 0.3F, 1.0F}});
	eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3D {
	    .position = {0.5F, -0.5F, 0.0F}, .rotation_deg = 60.0F, .size = {0.4F, 0.8F}, .color = {0.2F, 0.8F, 0.3F, 1.0F}});
	eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3DTexture {
	    .position = {0.0F, 0.0F, -0.1F}, .rotation_deg = 0.0F, .size = {10.0F, 10.0F}, .texture = checkerboard_texture_});
	eclipse::Renderer2D::end_scene();
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
