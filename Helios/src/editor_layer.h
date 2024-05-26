#pragma once

#include "eclipse.h"
#include "panels/scene_hierarchy_panel.h"
#include "panels/content_browser_panel.h"

namespace eclipse {

class EditorLayer : public Layer {
public:
	EditorLayer() : Layer("EditorLayer") {}

	virtual ~EditorLayer() = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void on_event(Event& event) override;
	void on_imgui_render() override;

private:
	void create_new_active_scene();
	void open_scene();
	void open_scene(const std::filesystem::path& path);
	void save_scene_as();
	void save_scene();
	void serialize_scene(ref<Scene> scene, const std::filesystem::path& path);
	bool on_key_pressed(KeyPressedEvent& event);
	bool on_mouse_button_pressed(MouseButtonPressedEvent& event);
	void render_overlay();
	void on_scene_play();
	void on_scene_simulate();
	void on_scene_stop();
	void on_duplicate_entity();
	void draw_ui_toolbar();
	void calculate_framerate(const au::QuantityF<au::Seconds>& timestep);

	enum class SceneState { edit, play, simulate };
	SceneState scene_state_ {SceneState::edit};

	OrthographicCameraController camera_controller_ {1280.0F / 720.0F, EnableCameraRotation::yes};

	ref<FrameBuffer> frame_buffer_;
	ref<Scene> active_scene_;
	ref<Scene> editor_scene_;
	std::filesystem::path editor_scene_path_;
	Entity hovered_entity_;
	ref<Texture2D> icon_play_;
	ref<Texture2D> icon_simulate_;
	ref<Texture2D> icon_stop_;
	unsigned int frame_rate_ = 0;
	EditorCamera editor_camera_ {
	    {.fov = au::degrees(30.0F), .aspect_ratio = 1.778F, .near_clip = 0.1F, .far_clip = 1000.0F}};

	WindowSize viewport_size_ {};
	glm::vec2 viewport_bounds_[2] {};
	bool viewport_focused_ {false};
	bool viewport_hovered_ {false};

	SceneHierarchyPanel scene_hierarchy_panel_;
	ContentBrowserPanel content_browser_panel_;

	bool show_physics_colliders_ {false};
	bool outline_selected_entity_ {true};

	int gizmo_type_ = -1;

	static constexpr std::string_view WINDOWS_FILE_DIALOG_FILTER = "Eclipse Scene(*.eclipse)\0 *.eclipse\0";
};

}  // namespace eclipse