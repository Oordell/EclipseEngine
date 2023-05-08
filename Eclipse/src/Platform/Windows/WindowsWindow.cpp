#include "ecpch.h"
#include "WindowsWindow.h"
#include "eclipse/Core.h"

namespace eclipse {

	static bool GLFW_initialized = false;

	Window* Window::create(const WindowProps& props) {
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) {
		init(props);
	}

	WindowsWindow::~WindowsWindow() {
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props) {
		data_.props = props;

		EC_CORE_INFO("Creating window \"{0}\" of size ({1}, {2})", props.title, props.window_size.width, props.window_size.height);

		if (!GLFW_initialized) {
			auto success = glfwInit();
			EC_CORE_ASSERT(success, "Could not initialize GLFW!");
			GLFW_initialized = true;
		}

		window_ = glfwCreateWindow(static_cast<int>(props.window_size.width), static_cast<int>(props.window_size.height), data_.props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(window_);
		glfwSetWindowUserPointer(window_, &data_);
		set_v_sync(true);
	}

	void WindowsWindow::shutdown() {
		glfwDestroyWindow(window_);
	}

	void WindowsWindow::on_update() {
		glfwPollEvents();
		glfwSwapBuffers(window_);
	}

	void WindowsWindow::set_v_sync(bool enabled) {
		int input = enabled ? 1 : 0;
		glfwSwapInterval(input);
		data_.v_sync = enabled;
	}

	bool WindowsWindow::is_v_sync() const {
		return data_.v_sync;
	}
}