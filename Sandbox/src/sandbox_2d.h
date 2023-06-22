#pragma once

#include "eclipse.h"

class Sandbox2D : public eclipse::Layer {
public:
	Sandbox2D() : Layer("Sandbox2D") {}

	virtual ~Sandbox2D() = default;
	void on_attach() override;
	void on_detach() override;
	void on_update(eclipse::Timestep timestep) override;
	void on_event(eclipse::Event& event) override;
	void on_imgui_render() override;

private:
	eclipse::OrthographicCameraController camera_controller_ {1280.0F / 720.0F, eclipse::EnableCameraRotation::yes};
	eclipse::ref<eclipse::VertexArray> square_vertex_array_ = eclipse::VertexArray::create();
	eclipse::ref<eclipse::Shader> flat_color_shader_ =
	    eclipse::Shader::create(eclipse::FilePath("assets/shaders/flat_color.glsl"));

	glm::vec4 square_color_  = {0.2F, 0.3F, 0.8F, 1.0F};
	unsigned int frame_rate_ = 0;
};