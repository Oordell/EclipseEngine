#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Sandbox2D::on_attach() {
	EC_PROFILE_FUNCTION();

	checkerboard_texture_   = eclipse::Texture2D::create("assets/textures/Checkerboard.png");
	olliver_ordell_texture_ = eclipse::Texture2D::create("assets/textures/olliver_ordell_logo.png");
	tiny_town_texture_      = eclipse::Texture2D::create("assets/games/tiny_town/Tilemap/tilemap_packed.png");
	tiny_town_sheet_ =
	    eclipse::make_ref<eclipse::TextureSheet>(eclipse::TextureSheetProperties {.texture            = tiny_town_texture_,
	                                                                              .sub_tile_width     = 16,
	                                                                              .sub_tile_height    = 16,
	                                                                              .sub_tile_spacing_x = 0,
	                                                                              .sub_tile_spacing_y = 0});

	EC_DEBUG(tiny_town_sheet_->get_debug_string());

	sub_texture_key_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = tiny_town_sheet_, .tile_index_x = 9, .tile_index_y = 1});
	sub_texture_door_ = eclipse::make_ref<eclipse::SubTexture2D>(
	    eclipse::SubTexture2DProperties {.texture_sheet = tiny_town_sheet_, .tile_index_x = 6, .tile_index_y = 4});

	eclipse::utils::Random::init();

	particle_props_.color_begin        = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
	particle_props_.color_end          = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
	particle_props_.size_begin         = 0.35f;
	particle_props_.size_variation     = 0.2f;
	particle_props_.size_end           = 0.0f;
	particle_props_.life_time          = 1.0f;
	particle_props_.velocity           = {0.0f, 0.0f};
	particle_props_.velocity_variation = {3.0f, 3.0f};
	particle_props_.position           = {0.0f, 0.0f};

	camera_controller_.set_zoom_level(5.0f);

	frame_buffer_ = eclipse::FrameBuffer::create({.width = 1280, .height = 720});
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
		frame_buffer_->bind();
		eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
		eclipse::RenderCommand::clear();
	}

	{
		EC_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0F;
		rotation += timestep * 1.0F;

		eclipse::Renderer2D::begin_scene(camera_controller_.get_camera());

		if (eclipse::InputManager::is_mouse_button_pressed(EC_MOUSE_BUTTON_LEFT)) {
			auto [x, y] = eclipse::InputManager::get_mouse_pose();
			auto width  = eclipse::Application::get().get_window().get_width();
			auto height = eclipse::Application::get().get_window().get_height();

			auto bounds              = camera_controller_.get_bounds();
			auto pos                 = camera_controller_.get_camera().get_position();
			x                        = (x / width) * bounds.get_width() - bounds.get_width() * 0.5f;
			y                        = bounds.get_height() * 0.5f - (y / height) * bounds.get_height();
			particle_props_.position = {x + pos.x, y + pos.y};
			for (int i = 0; i < 5; i++) {
				particle_system_.emit(particle_props_);
			}
		}

		particle_system_.on_update(timestep);
		particle_system_.on_render(camera_controller_.get_camera());

		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition2D {
		    .position = {-1.0F, 0.0F}, .rotation_rad = 0.0F, .size = {0.8F, 0.8F}, .color = {0.8F, 0.2F, 0.3F, 1.0F}});
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3D {.position     = {0.5F, -0.5F, 0.0F},
		                                     .rotation_rad = static_cast<float>(std::numbers::pi) / 4.0F,
		                                     .size         = {0.5F, 0.75F},
		                                     .color        = square_color_});
		eclipse::Renderer2D::draw_quad(eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, -0.2F},
		                                                                       .size          = {20.0F, 20.0F},
		                                                                       .tiling_factor = 10.0F,
		                                                                       .texture       = checkerboard_texture_});
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.0F},
		                                            .rotation_rad  = rotation,
		                                            .size          = {1.0F, 1.0F},
		                                            .tiling_factor = 10.0F,
		                                            .texture       = checkerboard_texture_,
		                                            .tint_color    = glm::vec4(1.0F, 0.8F, 0.8F, 1.0F)});
		eclipse::Renderer2D::draw_quad(
		    eclipse::QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.2F},
		                                            .rotation_rad  = rotation,
		                                            .size          = {1.0F, 1.0F},
		                                            .tiling_factor = 1.0F,
		                                            .texture       = olliver_ordell_texture_,
		                                            .tint_color    = glm::vec4(1.0F, 0.8F, 0.8F, 1.0F)});

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
				eclipse::Renderer2D::draw_quad({.position = {x, y, -0.1F}, .size = {0.45F, 0.45F}, .color = color});
			}
		}

		eclipse::Renderer2D::draw_quad({.position       = {0.0F, 1.0F, 0.5F},
		                                .size           = {sub_texture_key_->get_width(), sub_texture_key_->get_height()},
		                                .texture        = sub_texture_key_->get_texture(),
		                                .texture_coords = sub_texture_key_->get_texture_coords()});
		eclipse::Renderer2D::draw_quad({.position       = {1.0F, 1.0F, 0.5F},
		                                .size           = {sub_texture_door_->get_width(), sub_texture_door_->get_height()},
		                                .texture        = sub_texture_door_->get_texture(),
		                                .texture_coords = sub_texture_door_->get_texture_coords()});

		eclipse::Renderer2D::end_scene();
		frame_buffer_->unbind();
	}
}

void Sandbox2D::on_event(eclipse::Event& event) { camera_controller_.on_event(event); }

void Sandbox2D::on_imgui_render() {
	EC_PROFILE_FUNCTION();

	// Note: Switch this to true to enable dockspace
	static bool docking_enabled = true;
	if (docking_enabled) {
		static bool dockspace_open                = true;
		static bool opt_fullscreen_persistant     = true;
		bool opt_fullscreen                       = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into, because it would be
		// confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |=
			    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru
		// hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
			window_flags |= ImGuiWindowFlags_NoBackground;
		}

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed). This is because we want to
		// keep our DockSpace() active. If a DockSpace() is inactive, all active windows docked into it will lose their parent
		// and become undocked. We cannot preserve the docking relationship between an active window and an inactive docking,
		// otherwise any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen) {
			ImGui::PopStyleVar(2);
		}

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// Disabling fullscreen would allow the window to be moved to the front of other windows, which we can't undo at the
				// moment without finer window depth/z control. ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
				if (ImGui::MenuItem("Exit")) {
					eclipse::Application::get().close();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = eclipse::Renderer2D::get_statistics();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw calls: %d", stats.draw_calls);
		ImGui::Text("Quad count: %d", stats.quad_count);
		ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
		ImGui::Text("Indices   : %d", stats.get_total_index_count());

		ImGui::ColorEdit4("Square color", glm::value_ptr(square_color_));
		uint64_t texture_id = static_cast<uint64_t>(frame_buffer_->get_color_attachment_renderer_id());
		ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {1280.0F, 720.0F});

		ImGui::End();

		ImGui::End();
	} else {
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
		uint64_t texture_id = static_cast<uint64_t>(checkerboard_texture_->get_renderer_id());
		ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {256.0F, 256.0F});

		ImGui::End();
	}
}
