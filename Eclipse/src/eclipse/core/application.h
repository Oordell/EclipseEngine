#pragma once

#include "eclipse/core/core.h"
#include "eclipse/core/window.h"
#include "eclipse/layer/layer_stack.h"
#include "eclipse/events/event.h"
#include "eclipse/events/window_events.h"
#include "eclipse/imgui/imgui_layer.h"
#include "eclipse/core/timestep.h"

int main(int argc, char** argv);

namespace eclipse {

class ECLIPSE_API Application {
public:
	Application();
	virtual ~Application();
	void close();

	void on_event(Event& e);

	void push_layer(Layer* layer);

	void push_overlay(Layer* overlay);

	inline Window& get_window() { return *window_; }

	inline static Application& get() { return *instance_; }

private:
	void run();
	bool on_window_closed(WindowClosedEvent& e);
	bool on_window_resize(WindowResizeEvent& e);

	static Application* instance_;
	scope<Window> window_        = Window::create();
	ref<ImGuiLayer> imgui_layer_ = make_ref<ImGuiLayer>();
	bool running_                = true;
	bool minimized_              = false;
	LayerStack layer_stack_;
	float last_frame_time_ {};

	friend int ::main(int argc, char** argv);
};

// To be defined in client
Application* create_application();

}  // namespace eclipse