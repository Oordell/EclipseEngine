#pragma once

#include "eclipse.h"
#include "panels/scene_hierarchy_panel.h"

namespace eclipse {

class EditorLayer : public Layer {
public:
	EditorLayer() : Layer("EditorLayer") {}

	virtual ~EditorLayer() = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(Timestep timestep) override;
	void on_event(Event& event) override;
	void on_imgui_render() override;

private:
	void create_new_active_scene();
	void open_scene();
	void save_scene_as();
	bool on_key_pressed(KeyPressedEvent& event);

	OrthographicCameraController camera_controller_ {1280.0F / 720.0F, EnableCameraRotation::yes};

	ref<FrameBuffer> frame_buffer_;
	ref<Scene> active_scene_;
	Entity square_entity_;
	Entity red_square_entity_;
	Entity camera_entity_;
	Entity second_camera_;
	bool use_primary_camera_ = true;
	ref<Texture2D> checkerboard_texture_;
	ref<Texture2D> olliver_ordell_texture_;
	unsigned int frame_rate_ = 0;
	EditorCamera editor_camera_ {{.fov = 30.0F, .aspect_ratio = 1.778F, .near_clip = 0.1F, .far_clip = 1000.0F}};

	WindowSize viewport_size_ {.width = 0, .height = 0};
	glm::vec2 viewport_bounds_[2];
	bool viewport_focused_ {false};
	bool viewport_hovered_ {false};

	SceneHierarchyPanel scene_hierarchy_panel_;

	int gizmo_type_ = -1;

	static constexpr std::string_view WINDOWS_FILE_DIALOG_FILTER = "Eclipse Scene(*.eclipse)\0 *.eclipse\0";
};

}  // namespace eclipse