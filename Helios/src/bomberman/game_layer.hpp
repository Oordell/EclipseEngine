#pragma once

#include <Eclipse.h>

#include "../panels/scene_hierarchy_panel.h"
#include "level.hpp"

#include <imgui/imgui.h>
#include <au.hh>

namespace bomberman {

class GameLayer : public eclipse::Layer {
public:
	GameLayer() : Layer("Game Layer") {}

	virtual ~GameLayer() = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void on_event(eclipse::Event& event) override;
	void on_imgui_render() override;

private:
	void create_new_active_scene();
	void serialize_scene(eclipse::ref<eclipse::Scene> scene, const std::filesystem::path& path);
	bool on_key_pressed(eclipse::KeyPressedEvent& event);
	bool on_mouse_button_pressed(eclipse::MouseButtonPressedEvent& event);
	void render_overlay();
	void on_duplicate_entity();
	void on_play_game();
	void on_stop_game();
	void calculate_framerate(const au::QuantityF<au::Seconds>& timestep);
	void set_panel_context(eclipse::ref<eclipse::Scene> new_context);

	enum class SceneState { main_menu, play, pause, game_over };
	SceneState scene_state_ {SceneState::main_menu};
	Level level_;

	eclipse::OrthographicCameraController camera_controller_ {1280.0F / 720.0F, eclipse::EnableCameraRotation::yes};

	eclipse::ref<eclipse::FrameBuffer> frame_buffer_;
	eclipse::ref<eclipse::Scene> active_scene_;
	eclipse::ref<eclipse::Scene> editor_scene_;
	eclipse::Entity hovered_entity_;
	unsigned int frame_rate_ = 0;
	eclipse::EditorCamera editor_camera_ {
	    {.fov = au::degrees(30.0F), .aspect_ratio = 1.778F, .near_clip = 0.1F, .far_clip = 1000.0F}};

	eclipse::WindowSize viewport_size_ {};
	glm::vec2 viewport_bounds_[2] {};
	bool viewport_focused_ {false};
	bool viewport_hovered_ {false};

	eclipse::SceneHierarchyPanel scene_hierarchy_panel_;

	bool show_physics_colliders_ {false};
	bool outline_selected_entity_ {true};

	int gizmo_type_ = -1;
};

}  // namespace bomberman