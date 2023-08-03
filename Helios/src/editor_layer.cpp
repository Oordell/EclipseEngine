#include "editor_layer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

void EditorLayer::on_attach() {
	EC_PROFILE_FUNCTION();

	checkerboard_texture_   = Texture2D::create("assets/textures/Checkerboard.png");
	olliver_ordell_texture_ = Texture2D::create("assets/textures/olliver_ordell_logo.png");

	camera_controller_.set_zoom_level(5.0f);

	frame_buffer_ = FrameBuffer::create({.width = 1280, .height = 720});
}

void EditorLayer::on_detach() { EC_PROFILE_FUNCTION(); }

void EditorLayer::on_update(Timestep timestep) {
	EC_PROFILE_FUNCTION();

	if (auto specs = frame_buffer_->get_specification();
	    viewport_size_.width > 0 && viewport_size_.height > 0 &&
	    (specs.width != viewport_size_.width || specs.height != viewport_size_.height)) {
		frame_buffer_->resize(viewport_size_);
		camera_controller_.on_resize(viewport_size_);
	}

	if (viewport_focused_) {
		camera_controller_.on_update(timestep);
	}

	frame_rate_ = static_cast<unsigned int>(1.0F / timestep);
	EC_TRACE_THROTTLED(1.0, "Frame rate: {0}Hz", frame_rate_);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	Renderer2D::reset_statistics();
	{
		EC_PROFILE_SCOPE("Renderer Prep");
		frame_buffer_->bind();
		RenderCommand::set_clear_color({red, green, blue, alpha});
		RenderCommand::clear();
	}

	{
		EC_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0F;
		rotation += timestep * 1.0F;

		Renderer2D::begin_scene(camera_controller_.get_camera());

		Renderer2D::draw_quad(QuadMetaDataPosition2D {
		    .position = {-1.0F, 0.0F}, .rotation_rad = 0.0F, .size = {0.8F, 0.8F}, .color = {0.8F, 0.2F, 0.3F, 1.0F}});
		Renderer2D::draw_quad(QuadMetaDataPosition3D {.position     = {0.5F, -0.5F, 0.0F},
		                                              .rotation_rad = static_cast<float>(std::numbers::pi) / 4.0F,
		                                              .size         = {0.5F, 0.75F},
		                                              .color        = square_color_});
		Renderer2D::draw_quad(QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, -0.2F},
		                                                     .size          = {20.0F, 20.0F},
		                                                     .tiling_factor = 10.0F,
		                                                     .texture       = checkerboard_texture_});
		Renderer2D::draw_quad(QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.0F},
		                                                     .rotation_rad  = rotation,
		                                                     .size          = {1.0F, 1.0F},
		                                                     .tiling_factor = 10.0F,
		                                                     .texture       = checkerboard_texture_,
		                                                     .tint_color    = glm::vec4(1.0F, 0.8F, 0.8F, 1.0F)});
		Renderer2D::draw_quad(QuadMetaDataPosition3DTexture {.position      = {0.0F, 0.0F, 0.2F},
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
				Renderer2D::draw_quad({.position = {x, y, -0.1F}, .size = {0.45F, 0.45F}, .color = color});
			}
		}

		Renderer2D::end_scene();
		frame_buffer_->unbind();
	}
}

void EditorLayer::on_event(Event& event) { camera_controller_.on_event(event); }

void EditorLayer::on_imgui_render() {
	EC_PROFILE_FUNCTION();

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

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed). This is because we want
	// to keep our DockSpace() active. If a DockSpace() is inactive, all active windows docked into it will lose their
	// parent and become undocked. We cannot preserve the docking relationship between an active window and an inactive
	// docking, otherwise any change of dockspace/settings would lead to windows being stuck in limbo and never being
	// visible.
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
			// Disabling fullscreen would allow the window to be moved to the front of other windows, which we can't undo at
			// the moment without finer window depth/z control. ImGui::MenuItem("Fullscreen", NULL,
			// &opt_fullscreen_persistant);
			if (ImGui::MenuItem("Exit")) {
				Application::get().close();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Begin("Settings");

	auto stats = Renderer2D::get_statistics();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.draw_calls);
	ImGui::Text("Quad count: %d", stats.quad_count);
	ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
	ImGui::Text("Indices   : %d", stats.get_total_index_count());

	ImGui::ColorEdit4("Square color", glm::value_ptr(square_color_));
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0F, 0.0F});
	ImGui::Begin("Viewport");

	viewport_focused_ = ImGui::IsWindowFocused();
	viewport_hovered_ = ImGui::IsWindowHovered();
	Application::get().get_imgui_layer()->set_block_events(!(viewport_focused_ && viewport_hovered_));

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	uint32_t temp_width        = static_cast<uint32_t>(viewport_panel_size.x);
	uint32_t temp_height       = static_cast<uint32_t>(viewport_panel_size.y);
	if (viewport_size_.width != temp_width || viewport_size_.height != temp_height) {
		viewport_size_ = {.width = temp_width, .height = temp_height};
	}
	uint64_t texture_id = static_cast<uint64_t>(frame_buffer_->get_color_attachment_renderer_id());
	ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {viewport_panel_size.x, viewport_panel_size.y}, ImVec2 {0, 1},
	             ImVec2 {1, 0});
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}
}  // namespace eclipse