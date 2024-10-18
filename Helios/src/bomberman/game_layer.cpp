#include "game_layer.hpp"

namespace bomberman {

void GameLayer::on_attach() {
	EC_TRACE("GameLayer::on_attach()");

	auto command_line_args = eclipse::Application::get().get_command_line_args();
	if (command_line_args.count > 1) {
		EC_DEBUG("Loading command line arguments...");
		auto scene_file_path = std::filesystem::path(command_line_args[1]);
		eclipse::SceneSerializer serializer(active_scene_);
		serializer.deserialize_text(scene_file_path);
	}

	eclipse::Renderer2D::set_line_width(eclipse::units::pixels(4.F));
	active_scene_->on_viewport_resize(viewport_size_);
}

void GameLayer::on_detach() { EC_TRACE("GameLayer::on_detach()"); }

void GameLayer::on_update(au::QuantityF<au::Seconds> timestep) {
	EC_TRACE_THROTTLED(1.F, "GameLayer::on_update()");

	if (auto specs = frame_buffer_->get_specification();
	    viewport_size_.width > eclipse::units::pixels(0) && viewport_size_.height > eclipse::units::pixels(0) &&
	    (specs.width != viewport_size_.width || specs.height != viewport_size_.height)) {
		frame_buffer_->resize(viewport_size_);
		camera_controller_.on_resize(viewport_size_);

		active_scene_->on_viewport_resize(viewport_size_);
	}

	//	calculate_framerate(timestep);

	static constexpr float red   = 0.1F;
	static constexpr float green = 0.1F;
	static constexpr float blue  = 0.1F;
	static constexpr float alpha = 1.0F;

	eclipse::Renderer2D::reset_statistics();

	frame_buffer_->bind();
	eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
	eclipse::RenderCommand::clear();

	// Clear the entity ID attachment for the clear color to -1;
	frame_buffer_->clear_attachment(1, -1);

	active_scene_->on_update_runtime(timestep);

	/*switch (scene_state_) {
	 case SceneState::edit: {
	  active_scene_->on_update_editor(timestep, editor_camera_);
	  break;
	 }
	 case SceneState::simulate: {
	  editor_camera_.on_update(timestep);
	  active_scene_->on_update_simulation(timestep, editor_camera_);
	  break;
	 }
	 case SceneState::play: {
	  active_scene_->on_update_runtime(timestep);
	  break;
	 }
	}*/

	frame_buffer_->unbind();
}

void GameLayer::on_event(eclipse::Event& event) { EC_TRACE("GameLayer::on_event()"); }

void GameLayer::on_imgui_render() {
	EC_TRACE_THROTTLED(1.F, "GameLayer::on_imgui_render()");

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
	ImGuiIO& io           = ImGui::GetIO();
	ImGuiStyle& style     = ImGui::GetStyle();
	auto min_window_size  = style.WindowMinSize;
	style.WindowMinSize.x = 380.0F;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	style.WindowMinSize.x = min_window_size.x;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0F, 0.0F});
	ImGui::Begin("Viewport");
	auto viewport_min_region = ImGui::GetWindowContentRegionMin();
	auto viewport_max_region = ImGui::GetWindowContentRegionMax();
	auto viewport_offset     = ImGui::GetWindowPos();
	viewport_bounds_[0]      = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
	viewport_bounds_[1]      = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	auto temp_width            = eclipse::units::pixels(static_cast<uint32_t>(viewport_panel_size.x));
	auto temp_height           = eclipse::units::pixels(static_cast<uint32_t>(viewport_panel_size.y));
	if (viewport_size_.width != temp_width || viewport_size_.height != temp_height) {
		viewport_size_ = {.width = temp_width, .height = temp_height};
	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

}  // namespace bomberman