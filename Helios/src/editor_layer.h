#pragma once

#include "eclipse.h"

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
	OrthographicCameraController camera_controller_ {1280.0F / 720.0F, EnableCameraRotation::yes};

	ref<FrameBuffer> frame_buffer_;

	ref<Texture2D> checkerboard_texture_;
	ref<Texture2D> olliver_ordell_texture_;

	glm::vec4 square_color_  = {0.2F, 0.3F, 0.8F, 1.0F};
	unsigned int frame_rate_ = 0;

	WindowSize viewport_size_ {};
	bool viewport_focused_ {false};
	bool viewport_hovered_ {false};
};

}  // namespace eclipse