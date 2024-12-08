#include "ecpch.h"
#include "game_layer.hpp"

#include "imgui/imgui.h"
#include "ImGuizmo.h"
#include "eclipse/scene/scene_serializer.h"
#include "eclipse/utils/platform_utils.h"
#include "eclipse/utils/math.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace bomberman {

void GameLayer::on_attach() {
	EC_TRACE("GameLayer::on_attach()");

	frame_buffer_ = eclipse::FrameBuffer::create(
	    {.width       = eclipse::units::pixels(1600),
	     .height      = eclipse::units::pixels(900),
	     .attachments = {eclipse::FramebufferTextureFormat::rgba8, eclipse::FramebufferTextureFormat::red_integer,
	                     eclipse::FramebufferTextureFormat::depth}});

	create_new_active_scene();

	auto command_line_args = eclipse::Application::get().get_command_line_args();
	if (command_line_args.count > 1) {
		EC_DEBUG("Loading command line arguments...");
		auto scene_file_path = std::filesystem::path(command_line_args[1]);
		eclipse::SceneSerializer serializer(active_scene_);
		serializer.deserialize_text(scene_file_path);
	}

	eclipse::Renderer2D::set_line_width(eclipse::units::pixels(4.F));

	editor_camera_.set_position({7.F, 7.F, 0.F});
	editor_camera_.set_distance(35.F);
}

void GameLayer::on_detach() { EC_PROFILE_FUNCTION(); }

void GameLayer::on_update(au::QuantityF<au::Seconds> timestep) {
	EC_PROFILE_FUNCTION();

	if (auto specs = frame_buffer_->get_specification();
	    viewport_size_.width > eclipse::units::pixels(0) && viewport_size_.height > eclipse::units::pixels(0) &&
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

	eclipse::Renderer2D::reset_statistics();

	frame_buffer_->bind();
	eclipse::RenderCommand::set_clear_color({red, green, blue, alpha});
	eclipse::RenderCommand::clear();

	// Clear the entity ID attachment for the clear color to -1;
	frame_buffer_->clear_attachment(1, -1);

	level_.on_update(timestep);

	switch (scene_state_) {
		case SceneState::main_menu: {
			active_scene_->on_update_editor(timestep, editor_camera_);
			break;
		}
		case SceneState::play: {
			active_scene_->on_update_runtime(timestep);
			break;
		}
		case SceneState::pause: {
			break;
		}
		case SceneState::game_over: {
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
		hovered_entity_ = pixel_value == -1 ? eclipse::Entity()
		                                    : eclipse::Entity(static_cast<entt::entity>(pixel_value), active_scene_.get());
	}

	render_overlay();

	frame_buffer_->unbind();
}

void GameLayer::on_event(eclipse::Event& event) {
	camera_controller_.on_event(event);
	if (scene_state_ == SceneState::main_menu) {
		editor_camera_.on_event(event);
	}
	level_.on_event(event);

	eclipse::EventDispatcher dispatcher(event);
	dispatcher.dispatch<eclipse::KeyPressedEvent>(EC_BIND_EVENT_FN(GameLayer::on_key_pressed));
	dispatcher.dispatch<eclipse::MouseButtonPressedEvent>(EC_BIND_EVENT_FN(GameLayer::on_mouse_button_pressed));
}

void GameLayer::on_imgui_render() {
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

	scene_hierarchy_panel_.on_imgui_render();
	level_.on_imgui_render();

	ImGui::Begin("Stats");

	std::string name = hovered_entity_ ? hovered_entity_.get_component<eclipse::component::Tag>().tag : "None";
	ImGui::Text("Hovered Entity: %s", name.c_str());
	auto stats = eclipse::Renderer2D::get_statistics();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.draw_calls);
	ImGui::Text("Quad count: %d", stats.quad_count);
	ImGui::Text("Vertices  : %d", stats.get_total_vertex_count());
	ImGui::Text("Indices   : %d", stats.get_total_index_count());
	ImGui::Text("Framerate : %dHz", frame_rate_);
	ImGui::Separator();
	ImGui::End();

	std::string player_position =
	    "Player position: " + std::to_string(level_.get_player_position().x.in(eclipse::units::pixels)) + ", " +
	    std::to_string(level_.get_player_position().y.in(eclipse::units::pixels));

	ImGui::Begin("Settings");
	ImGui::Checkbox("Show physics colliders", &show_physics_colliders_);
	ImGui::Checkbox("Highlight selected entity", &outline_selected_entity_);
	ImGui::DragFloat("Player Speed", &level_.get_player_speed());
	ImGui::Text("Bomb reach:            %d", level_.get_bomb_reach());
	ImGui::Text("Total number of bombs: %d", level_.get_total_num_of_bombs());
	ImGui::Text(player_position.c_str());
	if (ImGui::Button("Play")) {
		on_play_game();
	}
	if (ImGui::Button("Stop")) {
		on_stop_game();
	}
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
	eclipse::Application::get().get_imgui_layer()->set_block_events(!viewport_focused_ && !viewport_hovered_);

	ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
	auto temp_width            = eclipse::units::pixels(static_cast<uint32_t>(viewport_panel_size.x));
	auto temp_height           = eclipse::units::pixels(static_cast<uint32_t>(viewport_panel_size.y));
	if (viewport_size_.width != temp_width || viewport_size_.height != temp_height) {
		viewport_size_ = {.width = temp_width, .height = temp_height};
	}
	uint64_t texture_id = static_cast<uint64_t>(frame_buffer_->get_color_attachment_renderer_id(0));
	ImGui::Image(reinterpret_cast<void*>(texture_id), ImVec2 {viewport_panel_size.x, viewport_panel_size.y}, ImVec2 {0, 1},
	             ImVec2 {1, 0});

	// Gizmos:
	auto selected_entity = scene_hierarchy_panel_.get_selected_entity();
	if (selected_entity && gizmo_type_ > -1) {
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto window_width  = viewport_bounds_[1].x - viewport_bounds_[0].x;
		auto window_height = viewport_bounds_[1].y - viewport_bounds_[0].y;
		ImGuizmo::SetRect(viewport_bounds_[0].x, viewport_bounds_[0].y, window_width, window_height);

		// Editor camera:
		const glm::mat4& camera_projection = editor_camera_.get_projection();
		glm::mat4 camera_view              = editor_camera_.get_view_matrix();

		auto& trans                = selected_entity.get_component<eclipse::component::Transform>();
		glm::mat4 entity_transform = trans.get_transform();

		// Snapping
		bool snap            = eclipse::InputManager::is_key_pressed(eclipse::KeyCode::left_control);
		float snap_value     = gizmo_type_ == ImGuizmo::OPERATION::ROTATE ? 45.0F : 0.5F;
		float snap_values[3] = {snap_value, snap_value, snap_value};

		ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
		                     static_cast<ImGuizmo::OPERATION>(gizmo_type_), ImGuizmo::LOCAL, glm::value_ptr(entity_transform),
		                     nullptr, snap ? snap_values : nullptr);

		if (ImGuizmo::IsUsing() && !eclipse::InputManager::is_key_pressed(eclipse::KeyCode::left_alt)) {
			auto decomposed_transform = eclipse::utils::decompose_transform(entity_transform);
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

void GameLayer::create_new_active_scene() {
	if (editor_scene_) {
		editor_scene_.reset();
	}
	if (active_scene_) {
		active_scene_.reset();
	}

	editor_scene_ = eclipse::make_ref<eclipse::Scene>();
	active_scene_ = editor_scene_;
	active_scene_->on_viewport_resize(viewport_size_);
	set_panel_context(active_scene_);
	auto camera_entity = active_scene_->create_entity("Camera");
	auto& cam          = camera_entity.add_component<eclipse::component::Camera>();
	cam.camera.set_orthographic_size(18.5F);

	camera_entity.get_component<eclipse::component::Transform>().translation.x = 7.F;
	camera_entity.get_component<eclipse::component::Transform>().translation.y = 7.F;

	level_.setup_level();
}

void GameLayer::serialize_scene(eclipse::ref<eclipse::Scene> scene, const std::filesystem::path& path) {
	eclipse::SceneSerializer serializer(scene);
	serializer.serialize_text(std::filesystem::path(path.string()));
}

bool GameLayer::on_key_pressed(eclipse::KeyPressedEvent& event) {
	if (event.get_repeate_count() > 0) {
		return false;
	}
	/*
	 bool control_pressed = eclipse::InputManager::is_key_pressed(eclipse::KeyCode::left_control) ||
	                        eclipse::InputManager::is_key_pressed(eclipse::KeyCode::right_control);
	 bool shift_pressed = eclipse::InputManager::is_key_pressed(eclipse::KeyCode::left_shift) ||
	                      eclipse::InputManager::is_key_pressed(eclipse::KeyCode::right_shift);

	 switch (event.get_key_code()) {
	  case eclipse::KeyCode::Q: {
	   gizmo_type_ = -1;
	   break;
	  }
	  case eclipse::KeyCode::W: {
	   gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);
	   break;
	  }
	  case eclipse::KeyCode::E: {
	   gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::ROTATE);
	   break;
	  }
	  case eclipse::KeyCode::R: {
	   gizmo_type_ = static_cast<int>(ImGuizmo::OPERATION::SCALE);
	   break;
	  }
	  default:
	   break;
	 }*/

	return false;
}

bool GameLayer::on_mouse_button_pressed(eclipse::MouseButtonPressedEvent& event) {
	if (event.get_mouse_button() == eclipse::MouseCode::button_left) {
		if (viewport_hovered_ && !ImGuizmo::IsOver() && !eclipse::InputManager::is_key_pressed(eclipse::KeyCode::left_alt)) {
			scene_hierarchy_panel_.set_selected_entity(hovered_entity_);
		}
	}
	return false;
}

void GameLayer::render_overlay() {
	if (scene_state_ == SceneState::play) {
		auto camera = active_scene_->get_primary_camera_entity();
		if (!camera) {
			return;
		}
		eclipse::Renderer2D::begin_scene(
		    {.projection = camera.get_component<eclipse::component::Camera>().camera.get_projection(),
		     .transform  = camera.get_component<eclipse::component::Transform>().get_transform()});
	} else {
		eclipse::Renderer2D::begin_scene(editor_camera_);
	}

	if (show_physics_colliders_) {
		glm::vec4 outline_color = {0.F, 1.F, 0.F, 1.F};
		auto box_view =
		    active_scene_
		        ->get_view_of_all_entities_of_type<eclipse::component::Transform, eclipse::component::BoxCollider2D>();
		for (auto entity : box_view) {
			auto [transform, box] = box_view.get<eclipse::component::Transform, eclipse::component::BoxCollider2D>(entity);
			glm::vec3 scale       = transform.scale * glm::vec3(box.size * 2.F, 1.F);
			auto trans =
			    eclipse::utils::create_transform_with_offset(transform.translation, transform.rotation, scale, box.offset);
			eclipse::Renderer2D::draw_rectangle({.transform = trans, .color = outline_color});
		}

		auto circle_view =
		    active_scene_
		        ->get_view_of_all_entities_of_type<eclipse::component::Transform, eclipse::component::CircleCollider2D>();
		for (auto entity : circle_view) {
			auto [transform, circle] =
			    circle_view.get<eclipse::component::Transform, eclipse::component::CircleCollider2D>(entity);
			glm::vec3 scale = transform.scale * glm::vec3(circle.radius.in(au::meters) *2.F);
			auto trans =
			    eclipse::utils::create_transform_with_offset(transform.translation, transform.rotation, scale, circle.offset);
			eclipse::Renderer2D::draw_circle(
			    {.transform = trans,
			     .component = {.color = outline_color, .radius = circle.radius, .thickness = au::unos(0.05F)}});
		}
	}

	if (outline_selected_entity_) {
		if (eclipse::Entity selected_entity = scene_hierarchy_panel_.get_selected_entity()) {
			auto transform_component = selected_entity.get_component<eclipse::component::Transform>();
			transform_component.translation.z += 0.002F;

			glm::vec4 outline_color = {1.F, 0.5F, 0.F, 1.F};

			if (selected_entity.has_component<eclipse::component::SpriteRenderer>() ||
			    selected_entity.has_component<eclipse::component::Color>() ||
			    selected_entity.has_component<eclipse::component::SubTexture>()) {
				eclipse::Renderer2D::draw_rectangle({.transform = transform_component.get_transform(), .color = outline_color});
			} else if (selected_entity.has_component<eclipse::component::CircleRenderer>()) {
				auto circle_component = selected_entity.get_component<eclipse::component::CircleRenderer>();
				eclipse::Renderer2D::draw_circle(
				    {.transform = transform_component.get_transform(),
				     .component = {.color = outline_color, .radius = circle_component.radius, .thickness = au::unos(0.05F)}});
			}
		}
	}

	eclipse::Renderer2D::end_scene();
}

void GameLayer::on_duplicate_entity() {
	if (scene_state_ != SceneState::main_menu) {
		return;
	}
	if (auto selected_entity = scene_hierarchy_panel_.get_selected_entity(); selected_entity) {
		editor_scene_->duplicate_entity(selected_entity);
	}
}

void GameLayer::on_play_game() {
	scene_state_ = SceneState::play;

	active_scene_->get_view_of_all_entities_of_type<eclipse::component::AllComponents>().each([this](auto e) {
		eclipse::Entity entity {e, active_scene_.get()};
		active_scene_->destroy_entity(entity);
	});
	active_scene_ = eclipse::Scene::copy(editor_scene_);
	set_panel_context(active_scene_);

	active_scene_->on_runtime_start();
}

void GameLayer::on_stop_game() {
	scene_state_ = SceneState::main_menu;

	active_scene_->on_runtime_stop();
	active_scene_->get_view_of_all_entities_of_type<eclipse::component::AllComponents>().each([this](auto e) {
		eclipse::Entity entity {e, active_scene_.get()};
		active_scene_->destroy_entity(entity);
	});
	active_scene_ = editor_scene_;

	set_panel_context(active_scene_);
}

void GameLayer::calculate_framerate(const au::QuantityF<au::Seconds>& timestep) {
	static constexpr au::QuantityF<au::Seconds> update_rate = au::milli(au::seconds)(250.F);
	static std::chrono::steady_clock::time_point last_hit;
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::microseconds>(now - last_hit).count() >=
	    (update_rate.in(au::seconds) *1'000'000.0F)) {
		last_hit    = std::chrono::steady_clock::now();
		frame_rate_ = static_cast<unsigned int>(1.0F / timestep.in(au::seconds));
	}
}

void GameLayer::set_panel_context(eclipse::ref<eclipse::Scene> new_context) {
	scene_hierarchy_panel_.set_context(new_context);
	level_.set_context(new_context);
}

}  // namespace bomberman