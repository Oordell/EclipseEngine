#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sandbox2D::on_attach() {
	EC_PROFILE_FUNCTION();

	checkerboard_texture_   = eclipse::Texture2D::create("assets/textures/Checkerboard.png");
	olliver_ordell_texture_ = eclipse::Texture2D::create("assets/textures/olliver_ordell_logo.png");
}

void Sandbox2D::on_detach() { EC_PROFILE_FUNCTION(); }

void Sandbox2D::on_update(eclipse::Timestep timestep) {
	EC_PROFILE_FUNCTION();

	camera_controller_.on_update(timestep);

	frame_rate_ = static_cast<unsigned int>(1.0F / timestep);
	EC_TRACE_THROTTLED(1.0, "Frame rate: {0}Hz", frame_rate_);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	eclipse::Renderer2D::reset_statistics();
	{
		EC_PROFILE_SCOPE("Renderer Prep");
		eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
		eclipse::RenderCommand::clear();
	}

	{
		EC_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0F;
		rotation += timestep * 40.0F;

		eclipse::Renderer2D::begin_scene(camera_controller_.get_camera());
		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition2D {
		    .position = {-1.0F, 0.0F}, .rotation_deg = 0.0F, .size = {0.8F, 0.8F}, .color = {0.8F, 0.2F, 0.3F, 1.0F}});
		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3D {.position     = {0.5F, -0.5F, 0.0F},
		                                                                .rotation_deg = 60.0F,
		                                                                .size         = {0.5F, 0.75F},
		                                                                .color        = {0.2F, 0.8F, 0.3F, 1.0F}});
		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, -0.1F},
		                                                                       .size          = {20.0F, 20.0F},
		                                                                       .tiling_factor = 10.0F,
		                                                                       .texture       = checkerboard_texture_});
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.0F},
		                                            .rotation_deg  = rotation,
		                                            .size          = {1.0F, 1.0F},
		                                            .tiling_factor = 10.0F,
		                                            .texture       = checkerboard_texture_,
		                                            .tint_color    = glm::vec4(1.0F, 0.8F, 0.8F, 1.0F)});
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.1F},
		                                            .rotation_deg  = rotation,
		                                            .size          = {1.0F, 1.0F},
		                                            .tiling_factor = 1.0F,
		                                            .texture       = olliver_ordell_texture_,
		                                            .tint_color    = glm::vec4(1.0F, 0.8F, 0.8F, 1.0F)});
		eclipse::Renderer2D::end_scene();

		eclipse::Renderer2D::begin_scene(camera_controller_.get_camera());

		static const int NUMBER = 10;
		float x                 = 0.0F;
		float y                 = 0.0F;
		float red               = 0.0F;
		float green             = 0.4F;
		float blue              = 0.0F;
		float alpha             = 0.8F;
		glm::vec4 color         = glm::vec4 {red, green, blue, alpha};
		for (int i = -NUMBER; i < NUMBER; i++) {
			y    = static_cast<float>(i) / 2;
			blue = (static_cast<float>(i) + NUMBER) / (2 * NUMBER);
			for (int j = -NUMBER; j < NUMBER; j++) {
				x     = static_cast<float>(j) / 2;
				red   = (static_cast<float>(j) + NUMBER) / (2 * NUMBER);
				color = {red, green, blue, alpha};
				eclipse::Renderer2D::draw_quad({.position = {x, y}, .size = {0.45F, 0.45F}, .color = color});
			}
		}

		eclipse::Renderer2D::end_scene();
	}
}

void Sandbox2D::on_event(eclipse::Event& event) { camera_controller_.on_event(event); }

void Sandbox2D::on_imgui_render() {
	EC_PROFILE_FUNCTION();

	std::string text = "Frame rate: " + std::to_string(frame_rate_);
	ImGui::Begin("Framerate");
	ImGui::Text(text.c_str());
	ImGui::End();

	ImGui::Begin("Settings");

	auto stats = eclipse::Renderer2D::get_statistics();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.draw_calls);
	ImGui::Text("Quad count: %d", stats.quad_count);
	ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
	ImGui::Text("Indices   : %d", stats.get_total_index_count());

	ImGui::ColorEdit4("Square color", glm::value_ptr(square_color_));

	ImGui::End();
}
