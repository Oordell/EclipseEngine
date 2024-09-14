#include "editor_layer.h"
#include "imgui/imgui.h"
#include "ImGuizmo.h"
#include "eclipse/scene/scene_serializer.h"
#include "eclipse/utils/platform_utils.h"
#include "eclipse/utils/math.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

void EditorLayer::on_attach() {
	EC_PROFILE_FUNCTION();

	icon_play_     = Texture2D::create("resources/icons/play_button.png");
	icon_simulate_ = Texture2D::create("resources/icons/simulate_button.png");
	icon_stop_     = Texture2D::create("resources/icons/stop_button.png");

	frame_buffer_ =
	    FrameBuffer::create({.width       = units::pixels(1600),
	                         .height      = units::pixels(900),
	                         .attachments = {FramebufferTextureFormat::rgba8, FramebufferTextureFormat::red_integer,
	                                         FramebufferTextureFormat::depth}});

	editor_scene_ = make_ref<Scene>();
	active_scene_ = editor_scene_;

	auto command_line_args = Application::get().get_command_line_args();
	if (command_line_args.count > 1) {
		EC_DEBUG("Loading command line arguments...");
		auto scene_file_path = std::filesystem::path(command_line_args[1]);
		SceneSerializer serializer(active_scene_);
		serializer.deserialize_text(scene_file_path);
	}

	set_panel_context(active_scene_);
	Renderer2D::set_line_width(units::pixels(4.F));
	texture_sheet_panel_.add_entity_destroyed_subscriber(&scene_hierarchy_panel_);
}

void EditorLayer::on_detach() {
	EC_PROFILE_FUNCTION();
	texture_sheet_panel_.remove_entity_destroyed_subscriber(&scene_hierarchy_panel_);
}

void EditorLayer::on_update(au::QuantityF<au::Seconds> timestep) {
	EC_PROFILE_FUNCTION();

	if (auto specs = frame_buffer_->get_specification();
	    viewport_size_.width > units::pixels(0) && viewport_size_.height > units::pixels(0) &&
	    (specs.width != viewport_size_.width || specs.height != viewport_size_.height)) {
		frame_buffer_->resize(viewport_size_);
		camera_controller_.on_resize(viewport_size_);
		editor_camera_.set_viewport_size(viewport_size_);

		active_scene_->on_viewport_resize(viewport_size_);
	}

	if (viewport_focused_) {
		camera_controller_.on_update(timestep);
	}
	editor_camera_.on_update(timestep);

	calculate_framerate(timestep);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	Renderer2D::reset_statistics();

	frame_buffer_->bind();
	RenderCommand::set_clear_color({red, green, blue, alpha});
	RenderCommand::clear();

	// Clear the entity ID attachment for the clear color to -1;
	frame_buffer_->clear_attachment(1, -1);

	switch (scene_state_) {
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
	}

	auto [m_x, m_y] = ImGui::GetMousePos();
	m_x -= viewport_bounds_[0].x;
	m_y -= viewport_bounds_[0].y;
	auto viewport_size = viewport_bounds_[1] - viewport_bounds_[0];
	m_y                = viewport_size.y - m_y;
	int mouse_x        = static_cast<int>(m_x);
	int mouse_y        = static_cast<int>(m_y);

	if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < static_cast<int>(viewport_size.x) &&
	    mouse_y < static_cast<int>(viewport_size.y)) {
		int pixel_value = frame_buffer_->get_pixel_value(1, mouse_x, mouse_y);
		hovered_entity_ = pixel_value == -1 ? Entity() : Entity(static_cast<entt::entity>(pixel_value), active_scene_.get());
	}

	render_overlay();

	frame_buffer_->unbind();
}

void EditorLayer::on_event(Event& event) {
	camera_controller_.on_event(event);
	if (scene_state_ == SceneState::edit) {
		editor_camera_.on_event(event);
	}

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<KeyPressedEvent>(EC_BIND_EVENT_FN(EditorLayer::on_key_pressed));
	dispatcher.dispatch<MouseButtonPressedEvent>(EC_BIND_EVENT_FN(EditorLayer::on_mouse_button_pressed));
}

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
	ImGuiIO& io           = ImGui::GetIO();
	ImGuiStyle& style     = ImGui::GetStyle();
	auto min_window_size  = style.WindowMinSize;
	style.WindowMinSize.x = 380.0F;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	style.WindowMinSize.x = min_window_size.x;

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			// Disabling fullscreen would allow the window to be moved to the front of other windows, which we can't undo at
			// the moment without finer window depth/z control. ImGui::MenuItem("Fullscreen", NULL,
			// &opt_fullscreen_persistant);
			if (ImGui::MenuItem("New", "Ctrl+N")) {
				create_new_active_scene();
			}

			if (ImGui::MenuItem("Open...", "Ctrl+O")) {
				open_scene();
			}

			if (ImGui::MenuItem("Save scene", "Ctrl+S")) {
				save_scene();
			}

			if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {
				save_scene_as();
			}

			if (ImGui::MenuItem("Exit")) {
				Application::get().close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Duplicate entity", "Ctrl+D", nullptr, scene_hierarchy_panel_.get_selected_entity())) {
				on_duplicate_entity();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	scene_hierarchy_panel_.on_imgui_render();
	content_browser_panel_.on_imgui_render();
	texture_sheet_panel_.on_imgui_render();

	ImGui::Begin("Stats");

	std::string name = hovered_entity_ ? hovered_entity_.get_component<component::Tag>().tag : "None";
	ImGui::Text("Hovered Entity: %s", name.c_str());

	auto stats = Renderer2D::get_statistics();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.draw_calls);
	ImGui::Text("Quad count: %d", stats.quad_count);
	ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
	ImGui::Text("Indices   : %d", stats.get_total_index_count());
	ImGui::Text("Framerate : %dHz", frame_rate_);

	ImGui::Separator();

	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::Checkbox("Show physics colliders", &show_physics_colliders_);
	ImGui::Checkbox("Highlight selected entity", &outline_selected_entity_);
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0F, 0.0F});
	ImGui::Begin("Viewport");
	auto viewport_min_region = ImGui::GetWindowContentRegionMin();
	auto viewport_max_region = ImGui::GetWindowContentRegionMax();
	auto viewport_offset     = ImGui::GetWindowPos();
	viewport_bounds_[0]      = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
	viewport_bounds_[1]      = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

	viewport_focused_ = ImGui::IsWindowFocused();
	viewport_hovered_ = ImGui::IsWindowHovered();
	Application::get().get_imgui_layer()->set_block_events(!viewport_focused_ && !viewport_hovered_);

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	auto temp_width            = units::pixels(static_cast<uint32_t>(viewport_panel_size.x));
	auto temp_height           = units::pixels(static_cast<uint32_t>(viewport_panel_size.y));
	if (viewport_size_.width != temp_width || viewport_size_.height != temp_height) {
		viewport_size_ = {.width = temp_width, .height = temp_height};
	}
	uint64_t texture_id = static_cast<uint64_t>(frame_buffer_->get_color_attachment_renderer_id(0));
	ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {viewport_panel_size.x, viewport_panel_size.y}, ImVec2 {0, 1},
	             ImVec2 {1, 0});

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ContentBrowserPanel::DRAG_DROP_ID)) {
			const wchar_t* path              = reinterpret_cast<const wchar_t*>(payload->Data);
			std::filesystem::path scene_path = std::filesystem::path(ContentBrowserPanel::ASSETS_DIRECTORY) / path;
			if (scene_path.has_extension() && scene_path.extension() == ".eclipse") {
				open_scene(scene_path);
			}
		}
		ImGui::EndDragDropTarget();
	}

	// Gizmos:
	auto selected_entity = scene_hierarchy_panel_.get_selected_entity();
	if (selected_entity && gizmo_type_ > -1) {
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto window_width  = viewport_bounds_[1].x - viewport_bounds_[0].x;
		auto window_height = viewport_bounds_[1].y - viewport_bounds_[0].y;
		ImGuizmo::SetRect(viewport_bounds_[0].x, viewport_bounds_[0].y, window_width, window_height);

		// Runtime camera from entity:
		// auto camera_entity                 = active_scene_->get_primary_camera_entity();
		// const auto& camera                 = camera_entity.get_component<component::Camera>().camera;
		// const glm::mat4& camera_projection = camera.get_projection();
		// glm::mat4 camera_view = glm::inverse(camera_entity.get_component<component::Transform>().get_transform());

		// Editor camera:
		const glm::mat4& camera_projection = editor_camera_.get_projection();
		glm::mat4 camera_view              = editor_camera_.get_view_matrix();

		auto& trans                = selected_entity.get_component<component::Transform>();
		glm::mat4 entity_transform = trans.get_transform();

		// Snapping
		bool snap            = InputManager::is_key_pressed(KeyCode::left_control);
		float snap_value     = gizmo_type_ == ImGuizmo::OPERATION::ROTATE ? 45.0F : 0.5F;
		float snap_values[3] = {snap_value, snap_value, snap_value};

		ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
		                     static_cast<ImGuizmo::OPERATION>(gizmo_type_), ImGuizmo::LOCAL, glm::value_ptr(entity_transform),
		                     nullptr, snap ? snap_values : nullptr);

		if (ImGuizmo::IsUsing() && !InputManager::is_key_pressed(KeyCode::left_alt)) {
			auto decomposed_transform = utils::decompose_transform(entity_transform);
			if (decomposed_transform.has_value()) {
				glm::vec3 delta_rotation = decomposed_transform.value().rotation - trans.rotation;
				trans.translation        = decomposed_transform.value().translation;
				trans.rotation += delta_rotation;
				trans.scale = decomposed_transform.value().scale;
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	draw_ui_toolbar();

	ImGui::End();
}

void EditorLayer::create_new_active_scene() {
	editor_scene_ = make_ref<Scene>();
	active_scene_ = editor_scene_;
	active_scene_->on_viewport_resize(viewport_size_);
	set_panel_context(active_scene_);
	editor_scene_path_ = std::filesystem::path();
}

void EditorLayer::open_scene() {
	auto result = FileDialogs::open_file(WINDOWS_FILE_DIALOG_FILTER.data());

	if (result.has_value()) {
		open_scene(result.value());
	} else {
		EC_CORE_ERROR("Couldn't open scene, as we didn't find a file path...");
	}
}

void EditorLayer::open_scene(const std::filesystem::path& path) {
	if (scene_state_ != SceneState::edit) {
		on_scene_stop();
	}

	if (!path.has_extension() || path.extension().string() != ".eclipse") {
		EC_ERROR("Couldn't load \"{}\". It's not a \".eclipse\" file.", path.filename().string());
		return;
	}
	EC_CORE_DEBUG("Opening file: \"{}\"", path.string());

	ref<Scene> new_scene = make_ref<Scene>();
	new_scene->on_viewport_resize(viewport_size_);
	SceneSerializer serializer(new_scene);
	if (serializer.deserialize_text(path)) {
		editor_scene_ = new_scene;
		editor_scene_->on_viewport_resize(viewport_size_);
		set_panel_context(editor_scene_);
		active_scene_      = editor_scene_;
		editor_scene_path_ = path;
	}
}

void EditorLayer::save_scene_as() {
	auto result = FileDialogs::save_file(WINDOWS_FILE_DIALOG_FILTER.data());

	if (result.has_value()) {
		serialize_scene(active_scene_, result.value());
		editor_scene_path_ = result.value();
	} else {
		EC_CORE_ERROR("Couldn't save scene, as we didn't find a file path...");
	}
}

void EditorLayer::save_scene() {
	if (!editor_scene_path_.empty()) {
		serialize_scene(active_scene_, editor_scene_path_);
	} else {
		save_scene_as();
	}
}

void EditorLayer::serialize_scene(ref<Scene> scene, const std::filesystem::path& path) {
	SceneSerializer serializer(scene);
	serializer.serialize_text(std::filesystem::path(path.string()));
}

bool EditorLayer::on_key_pressed(KeyPressedEvent& event) {
	if (event.get_repeate_count() > 0) {
		return false;
	}

	bool control_pressed =
	    InputManager::is_key_pressed(KeyCode::left_control) || InputManager::is_key_pressed(KeyCode::right_control);
	bool shift_pressed =
	    InputManager::is_key_pressed(KeyCode::left_shift) || InputManager::is_key_pressed(KeyCode::right_shift);

	switch (event.get_key_code()) {
		case KeyCode::N: {
			if (control_pressed) {
				create_new_active_scene();
			}
			break;
		}
		case KeyCode::O: {
			if (control_pressed) {
				open_scene();
			}
			break;
		}
		case KeyCode::S: {
			if (control_pressed) {
				if (shift_pressed) {
					save_scene_as();
				} else {
					save_scene();
				}
			}
			break;
		}
		case KeyCode::D: {
			if (control_pressed) {
				on_duplicate_entity();
			}
			break;
		}
		case KeyCode::Q: {
			gizmo_type_ = -1;
			break;
		}
		case KeyCode::W: {
			gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);
			break;
		}
		case KeyCode::E: {
			gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::ROTATE);
			break;
		}
		case KeyCode::R: {
			gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::SCALE);
			break;
		}
		default:
			break;
	}

	return false;
}

bool EditorLayer::on_mouse_button_pressed(MouseButtonPressedEvent& event) {
	if (event.get_mouse_button() == MouseCode::button_left) {
		if (viewport_hovered_ && !ImGuizmo::IsOver() && !InputManager::is_key_pressed(KeyCode::left_alt)) {
			scene_hierarchy_panel_.set_selected_entity(hovered_entity_);
		}
	}
	return false;
}

void EditorLayer::render_overlay() {
	if (scene_state_ == SceneState::play) {
		auto camera = active_scene_->get_primary_camera_entity();
		if (!camera) {
			return;
		}
		Renderer2D::begin_scene({.projection = camera.get_component<component::Camera>().camera.get_projection(),
		                         .transform  = camera.get_component<component::Transform>().get_transform()});
	} else {
		Renderer2D::begin_scene(editor_camera_);
	}

	if (show_physics_colliders_) {
		glm::vec4 outline_color = {0.F, 1.F, 0.F, 1.F};
		auto box_view = active_scene_->get_view_of_all_entities_of_type<component::Transform, component::BoxCollider2D>();
		for (auto entity : box_view) {
			auto [transform, box] = box_view.get<component::Transform, component::BoxCollider2D>(entity);
			glm::vec3 scale       = transform.scale * glm::vec3(box.size * 2.F, 1.F);
			auto trans = utils::create_transform_with_offset(transform.translation, transform.rotation, scale, box.offset);
			Renderer2D::draw_rectangle({.transform = trans, .color = outline_color});
		}

		auto circle_view =
		    active_scene_->get_view_of_all_entities_of_type<component::Transform, component::CircleCollider2D>();
		for (auto entity : circle_view) {
			auto [transform, circle] = circle_view.get<component::Transform, component::CircleCollider2D>(entity);
			glm::vec3 scale          = transform.scale * glm::vec3(circle.radius.in(au::meters) *2.F);
			auto trans = utils::create_transform_with_offset(transform.translation, transform.rotation, scale, circle.offset);
			Renderer2D::draw_circle(
			    {.transform = trans,
			     .component = {.color = outline_color, .radius = circle.radius, .thickness = au::unos(0.05F)}});
		}
	}

	if (outline_selected_entity_) {
		if (Entity selected_entity = scene_hierarchy_panel_.get_selected_entity()) {
			auto transform_component = selected_entity.get_component<component::Transform>();
			transform_component.translation.z += 0.002F;

			glm::vec4 outline_color = {1.F, 0.5F, 0.F, 1.F};

			if (selected_entity.has_component<component::SpriteRenderer>() ||
			    selected_entity.has_component<component::Color>() || selected_entity.has_component<component::SubTexture>()) {
				Renderer2D::draw_rectangle({.transform = transform_component.get_transform(), .color = outline_color});
			} else if (selected_entity.has_component<component::CircleRenderer>()) {
				auto circle_component = selected_entity.get_component<component::CircleRenderer>();
				Renderer2D::draw_circle(
				    {.transform = transform_component.get_transform(),
				     .component = {.color = outline_color, .radius = circle_component.radius, .thickness = au::unos(0.05F)}});
			}
		}
	}

	Renderer2D::end_scene();
}

void EditorLayer::on_scene_play() {
	if (scene_state_ == SceneState::simulate) {
		on_scene_stop();
	}
	scene_state_ = SceneState::play;

	active_scene_ = Scene::copy(editor_scene_);
	active_scene_->on_runtime_start();

	set_panel_context(active_scene_);
}

void EditorLayer::on_scene_simulate() {
	if (scene_state_ == SceneState::play) {
		on_scene_stop();
	}
	scene_state_  = SceneState::simulate;
	active_scene_ = Scene::copy(editor_scene_);
	active_scene_->on_simulation_start();
	set_panel_context(active_scene_);
}

void EditorLayer::on_scene_stop() {
	if (scene_state_ == SceneState::play) {
		active_scene_->on_runtime_stop();
	} else if (scene_state_ == SceneState::simulate) {
		active_scene_->on_simulation_stop();
	}

	scene_state_ = SceneState::edit;

	active_scene_->on_runtime_stop();
	active_scene_ = editor_scene_;

	set_panel_context(active_scene_);
}

void EditorLayer::on_duplicate_entity() {
	if (scene_state_ != SceneState::edit) {
		return;
	}
	if (auto selected_entity = scene_hierarchy_panel_.get_selected_entity(); selected_entity) {
		editor_scene_->duplicate_entity(selected_entity);
	}
}

void EditorLayer::draw_ui_toolbar() {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	auto& colors               = ImGui::GetStyle().Colors;
	const auto& button_hovered = colors[ImGuiCol_ButtonHovered];
	const auto& button_active  = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(button_hovered.x, button_hovered.y, button_hovered.z, .5F));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(button_active.x, button_active.y, button_active.z, .5F));

	ImGui::Begin("##toolbar", nullptr,
	             ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	float icon_size    = ImGui::GetWindowHeight() - 4.F;
	auto play_icon     = (scene_state_ != SceneState::play) ? icon_play_ : icon_stop_;
	auto simulate_icon = (scene_state_ != SceneState::simulate) ? icon_simulate_ : icon_stop_;
	auto play_id       = static_cast<uint64_t>(play_icon->get_renderer_id());
	auto simulate_id   = static_cast<uint64_t>(simulate_icon->get_renderer_id());
	ImGui::SameLine((ImGui::GetWindowContentRegionMax().x * .5F) - (icon_size * .5F));
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(play_id), ImVec2(icon_size, icon_size), ImVec2(0, 0),
	                       ImVec2(1, 1), 0)) {
		switch (scene_state_) {
			case SceneState::edit:
				[[fallthrough]];
			case SceneState::simulate: {
				on_scene_play();
				break;
			}
			case SceneState::play: {
				on_scene_stop();
				break;
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(simulate_id), ImVec2(icon_size, icon_size), ImVec2(0, 0),
	                       ImVec2(1, 1), 0)) {
		switch (scene_state_) {
			case SceneState::edit:
				[[fallthrough]];
			case SceneState::play: {
				on_scene_simulate();
				break;
			}
			case SceneState::simulate: {
				on_scene_stop();
				break;
			}
		}
	}
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
	ImGui::End();
}

void EditorLayer::calculate_framerate(const au::QuantityF<au::Seconds>& timestep) {
	static constexpr au::QuantityF<au::Seconds> update_rate = au::milli(au::seconds)(250.F);
	static std::chrono::steady_clock::time_point last_hit;
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >=
	    (update_rate.in(au::seconds) *1'000'000.0F)) {
		last_hit    = std::chrono::steady_clock::now();
		frame_rate_ = static_cast<unsigned int>(1.0F / timestep.in(au::seconds));
	}
}

void EditorLayer::set_panel_context(ref<Scene> new_context) {
	scene_hierarchy_panel_.set_context(new_context);
	texture_sheet_panel_.set_context(new_context);
}
}  // namespace eclipse