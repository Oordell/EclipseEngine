#pragma once

#include <Eclipse.h>

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
	enum class SceneState { init, play, pause, end };
	SceneState scene_state_ {SceneState::init};

	eclipse::OrthographicCameraController camera_controller_ {1280.0F / 720.0F, eclipse::EnableCameraRotation::yes};

	eclipse::ref<eclipse::FrameBuffer> frame_buffer_ = eclipse::FrameBuffer::create(
	    {.width       = eclipse::units::pixels(1600),
	     .height      = eclipse::units::pixels(900),
	     .attachments = {eclipse::FramebufferTextureFormat::rgba8, eclipse::FramebufferTextureFormat::red_integer,
	                     eclipse::FramebufferTextureFormat::depth}});
	eclipse::ref<eclipse::Scene> active_scene_ = eclipse::make_ref<eclipse::Scene>();
	/*const eclipse::ref<eclipse::Texture2D> icon_play_ = eclipse::Texture2D::create("resources/icons/play_button.png");
	const eclipse::ref<eclipse::Texture2D> icon_stop_ = eclipse::Texture2D::create("resources/icons/stop_button.png");*/
	eclipse::WindowSize viewport_size_ {};
	glm::vec2 viewport_bounds_[2] {};
	unsigned int frame_rate_ = 0;
	bool viewport_focused_ {false};
	bool viewport_hovered_ {false};
};

}  // namespace bomberman