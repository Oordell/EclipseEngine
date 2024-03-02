#pragma once

#include "eclipse.h"

class Sandbox2D : public eclipse::Layer {
public:
	Sandbox2D() : Layer("Sandbox2D") {}

	virtual ~Sandbox2D() = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void on_event(eclipse::Event& event) override;
	void on_imgui_render() override;

private:
	eclipse::OrthographicCameraController camera_controller_ {1280.0F / 720.0F, eclipse::EnableCameraRotation::yes};
	eclipse::ref<eclipse::Texture2D> checkerboard_texture_;
	eclipse::ref<eclipse::Texture2D> olliver_ordell_texture_;
	eclipse::ref<eclipse::Texture2D> tiny_town_texture_;
	eclipse::ref<eclipse::TextureSheet> tiny_town_sheet_;
	eclipse::ref<eclipse::SubTexture2D> sub_texture_key_;
	eclipse::ref<eclipse::SubTexture2D> sub_texture_door_;

	glm::vec4 square_color_  = {0.2F, 0.3F, 0.8F, 1.0F};
	unsigned int frame_rate_ = 0;

	eclipse::component::ParticleSystem particle_system_ {};
	eclipse::component::ParticleProperties particle_props_ {};
};