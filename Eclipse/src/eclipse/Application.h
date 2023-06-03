#pragma once

#include "core.h"
#include "window.h"
#include "eclipse/layer/layer_stack.h"
#include "events/event.h"
#include "events/window_events.h"
#include "eclipse/imgui/imgui_layer.h"

#include <memory>

#include "eclipse/renderer/shader.h"
#include "eclipse/renderer/vertex_buffer.h"
#include "eclipse/renderer/index_buffer.h"
#include "eclipse/renderer/buffer_layout.h"
#include "eclipse/renderer/vertex_array.h"
#include "eclipse/renderer/orthographic_camera.h"

namespace eclipse {

class ECLIPSE_API Application {
public:
	Application();
	virtual ~Application();

	void run();

	void on_event(Event& e);

	void push_layer(Layer* layer);

	void push_overlay(Layer* overlay);

	inline Window& get_window() { return *window_; }

	inline static Application& get() { return *instance_; }

private:
	bool on_window_closed(WindowClosedEvent& e);
	bool on_mose_scrolled(MouseScrolledEvent& e);

	static Application* instance_;
	std::unique_ptr<Window> window_          = std::unique_ptr<Window>(Window::create());
	std::shared_ptr<ImGuiLayer> imgui_layer_ = std::make_shared<ImGuiLayer>();
	bool running_                            = true;
	LayerStack layer_stack_;

	std::shared_ptr<Shader> shader_;
	std::shared_ptr<VertexArray> vertex_array_ = std::shared_ptr<VertexArray>(VertexArray::create());

	std::shared_ptr<Shader> blue_shader_;
	std::shared_ptr<VertexArray> square_vertex_array_ = std::shared_ptr<VertexArray>(VertexArray::create());

	glm::vec3 camera_position_ {0.0F, 0.0F, 0.0F};
	float camera_rotation_ {0.0F};
	OrthographicCamera camera_ {{.left = -1.6F, .right = 1.6F, .bottom = -0.9, .top = 0.9}};
};

// To be defined in client
Application* create_application();

}  // namespace eclipse