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

	checkerboard_texture_   = Texture2D::create("assets/textures/Checkerboard.png");
	olliver_ordell_texture_ = Texture2D::create("assets/textures/olliver_ordell_logo.png");

	frame_buffer_ = FrameBuffer::create({.width       = 1600,
	                                     .height      = 900,
	                                     .attachments = {FramebufferTextureFormat::rgba8, FramebufferTextureFormat::rgba8,
	                                                     FramebufferTextureFormat::depth}});

	active_scene_ = make_ref<Scene>();
	/*
	square_entity_ = active_scene_->create_entity("Green square");
	square_entity_.add_component<component::Color>(glm::vec4 {0.2F, 0.9F, 0.3F, 1.0F});

	red_square_entity_ = active_scene_->create_entity("Red square");
	red_square_entity_.add_component<component::SpriteRenderer>(glm::vec4 {0.9F, 0.2F, 0.3F, 1.0F});

	camera_entity_ = active_scene_->create_entity("Camera Entity");
	camera_entity_.add_component<component::Camera>();

	second_camera_ = active_scene_->create_entity("Second camera");
	auto& cam      = second_camera_.add_component<component::Camera>();
	cam.primary    = false;

	class CameraController : public ScriptableEntity {
	public:
	 ~CameraController() override = default;

	 void on_create() override {
	  auto& transform         = get_component<component::Transform>();
	  transform.translation.x = rand() % 10 - 5.0F;
	 }

	 void on_destroy() override {}

	 void on_update(Timestep timestep) override {
	  auto& translation = get_component<component::Transform>().translation;

	  static const float camera_move_speed = 5.0F;
	  if (InputManager::is_key_pressed(KeyCode::A)) {
	   translation.x -= camera_move_speed * timestep;
	  } else if (InputManager::is_key_pressed(KeyCode::D)) {
	   translation.x += camera_move_speed * timestep;
	  }
	  if (InputManager::is_key_pressed(KeyCode::W)) {
	   translation.y += camera_move_speed * timestep;
	  } else if (InputManager::is_key_pressed(KeyCode::S)) {
	   translation.y -= camera_move_speed * timestep;
	  }
	 }
	};

	camera_entity_.add_component<component::NativeScript>().bind<CameraController>();
	second_camera_.add_component<component::NativeScript>().bind<CameraController>();

	*/

	scene_hierarchy_panel_.set_context(active_scene_);
}

void EditorLayer::on_detach() { EC_PROFILE_FUNCTION(); }

void EditorLayer::on_update(Timestep timestep) {
	EC_PROFILE_FUNCTION();

	if (auto specs = frame_buffer_->get_specification();
	    viewport_size_.width > 0 && viewport_size_.height > 0 &&
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

	frame_rate_ = static_cast<unsigned int>(1.0F / timestep);
	EC_TRACE_THROTTLED(1.0, "Frame rate: {0}Hz", frame_rate_);

	static const float red   = 0.1F;
	static const float green = 0.1F;
	static const float blue  = 0.1F;
	static const float alpha = 1.0F;

	Renderer2D::reset_statistics();

	frame_buffer_->bind();
	RenderCommand::set_clear_color({red, green, blue, alpha});
	RenderCommand::clear();

	active_scene_->on_update_editor(timestep, editor_camera_);

	frame_buffer_->unbind();
}

void EditorLayer::on_event(Event& event) {
	camera_controller_.on_event(event);
	editor_camera_.on_event(event);

	EventDispatcher dispatcher(event);
	dispatcher.dispatch<KeyPressedEvent>(EC_BIND_EVENT_FN(EditorLayer::on_key_pressed));
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

			if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {
				save_scene_as();
			}

			if (ImGui::MenuItem("Exit")) {
				Application::get().close();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	scene_hierarchy_panel_.on_imgui_render();

	ImGui::Begin("Settings");

	auto stats = Renderer2D::get_statistics();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.draw_calls);
	ImGui::Text("Quad count: %d", stats.quad_count);
	ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
	ImGui::Text("Indices   : %d", stats.get_total_index_count());

	ImGui::Separator();

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0F, 0.0F});
	ImGui::Begin("Viewport");

	viewport_focused_ = ImGui::IsWindowFocused();
	viewport_hovered_ = ImGui::IsWindowHovered();
	Application::get().get_imgui_layer()->set_block_events(!viewport_focused_ && !viewport_hovered_);

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	uint32_t temp_width        = static_cast<uint32_t>(viewport_panel_size.x);
	uint32_t temp_height       = static_cast<uint32_t>(viewport_panel_size.y);
	if (viewport_size_.width != temp_width || viewport_size_.height != temp_height) {
		viewport_size_ = {.width = temp_width, .height = temp_height};
	}
	uint64_t texture_id = static_cast<uint64_t>(frame_buffer_->get_color_attachment_renderer_id());
	ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {viewport_panel_size.x, viewport_panel_size.y}, ImVec2 {0, 1},
	             ImVec2 {1, 0});

	// Gizmos:
	auto selected_entity = scene_hierarchy_panel_.get_selected_entity();
	if (selected_entity && gizmo_type_ > -1) {
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto window_width  = static_cast<float>(ImGui::GetWindowWidth());
		auto window_height = static_cast<float>(ImGui::GetWindowHeight());
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, window_width, window_height);

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

		if (ImGuizmo::IsUsing()) {
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

	ImGui::End();
}

void EditorLayer::create_new_active_scene() {
	active_scene_ = make_ref<Scene>();
	active_scene_->on_viewport_resize(viewport_size_);
	scene_hierarchy_panel_.set_context(active_scene_);
}

void EditorLayer::open_scene() {
	auto result = FileDialogs::open_file(WINDOWS_FILE_DIALOG_FILTER.data());

	if (result.success) {
		create_new_active_scene();

		SceneSerializer serializer(active_scene_);
		serializer.deserialize_text(result.file_path.value());
	}
}

void EditorLayer::save_scene_as() {
	auto result = FileDialogs::save_file(WINDOWS_FILE_DIALOG_FILTER.data());

	if (result.success) {
		SceneSerializer serializer(active_scene_);
		serializer.serialize_text(result.file_path.value());
	}
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
			if (control_pressed && shift_pressed) {
				save_scene_as();
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
}  // namespace eclipse