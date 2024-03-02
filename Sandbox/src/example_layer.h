#pragma once

#include "eclipse.h"

class ExampleLayer : public eclipse::Layer {
public:
	ExampleLayer();
	~ExampleLayer() = default;
	void on_update(au::QuantityF<au::Seconds> timestep) override;
	void on_imgui_render() override;
	void on_event(eclipse::Event& event);

	void on_attach() override {};

	void on_detach() override {}

	bool on_key_pressed_event(eclipse::KeyPressedEvent& event) { return false; }

private:
	eclipse::ShaderLibrary shader_library_;
	eclipse::ref<eclipse::Shader> shader_;
	eclipse::ref<eclipse::VertexArray> vertex_array_ = eclipse::VertexArray::create();

	eclipse::ref<eclipse::Shader> flat_color_shader_;
	eclipse::ref<eclipse::VertexArray> square_vertex_array_ = eclipse::VertexArray::create();

	eclipse::ref<eclipse::Texture2D> texture_ = eclipse::Texture2D::create("assets/textures/Checkerboard.png");
	eclipse::ref<eclipse::Texture2D> texture_ordell_logo_ =
	    eclipse::Texture2D::create("assets/textures/olliver_ordell_logo.png");

	eclipse::OrthographicCameraController camera_controller_ {1280.0F / 720.0F, eclipse::EnableCameraRotation::yes};
	unsigned int frame_rate_ = 0;

	glm::vec3 square_color_ = {0.2F, 0.3F, 0.8F};
};