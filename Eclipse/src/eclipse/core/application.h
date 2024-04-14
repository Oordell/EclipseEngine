#pragma once

#include "eclipse/core/core.h"
#include "eclipse/core/window.h"
#include "eclipse/layer/layer_stack.h"
#include "eclipse/events/event.h"
#include "eclipse/events/window_events.h"
#include "eclipse/imgui/imgui_layer.h"

int main(int argc, char** argv);

namespace eclipse {

struct ApplicationCommandLineArgs {
	int count   = 0;
	char** args = nullptr;

	[[nodiscard]] const char* operator[](int index) const {
		EC_CORE_ASSERT(index < count || index >= 0, "Index less than count! index = {0}, count = {1}", index, count);
		return args[index];
	}
};

class ECLIPSE_API Application {
public:
	Application(const WindowProps& window_properties, const ApplicationCommandLineArgs& args = {});
	virtual ~Application();
	void close();

	void on_event(Event& e);

	void push_layer(Layer* layer);

	void push_overlay(Layer* overlay);

	ref<ImGuiLayer> get_imgui_layer() { return imgui_layer_; }

	inline Window& get_window() { return *window_; }

	inline static Application& get() { return *instance_; }

	[[nodiscard]] ApplicationCommandLineArgs get_command_line_args() const { return command_line_args_; }

private:
	void run();
	bool on_window_closed(WindowClosedEvent& e);
	bool on_window_resize(WindowResizeEvent& e);

	static Application* instance_;
	ref<ImGuiLayer> imgui_layer_ = make_ref<ImGuiLayer>();
	scope<Window> window_;
	bool running_   = true;
	bool minimized_ = false;
	LayerStack layer_stack_;
	float last_frame_time_ {};
	ApplicationCommandLineArgs command_line_args_ {};

	friend int ::main(int argc, char** argv);
};

// To be defined in client
Application* create_application(const ApplicationCommandLineArgs& args);

}  // namespace eclipse