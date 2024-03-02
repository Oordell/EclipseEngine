#pragma once

#include "eclipse/core/window.h"
#include "eclipse/renderer/graphics_context.h"

#include <GLFW/glfw3.h>

namespace eclipse {

class WindowsWindow : public Window {
public:
	WindowsWindow(const WindowProps& props);
	virtual ~WindowsWindow();

	void on_update() override;

	inline au::Quantity<units::Pixels, uint32_t> get_width() const override { return data_.props.window_size.width; }

	inline au::Quantity<units::Pixels, uint32_t> get_height() const override { return data_.props.window_size.height; }

	inline void set_event_callback(const EventCallbackfn& callback) override { data_.event_callback = callback; }

	void set_v_sync(bool enabled) override;
	bool is_v_sync() const override;

	inline void* get_native_window() const override { return window_; }

private:
	virtual void init(const WindowProps& props);
	virtual void shutdown();
	void set_glfw_callbacks();

	GLFWwindow* window_;
	scope<GraphicsContext> context_;

	struct WindowData {
		WindowProps props {};
		bool v_sync {false};

		EventCallbackfn event_callback;
	};

	WindowData data_;
};

}  // namespace eclipse