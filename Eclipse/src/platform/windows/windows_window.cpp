#include "ecpch.h"
#include "windows_window.h"
#include "eclipse/core.h"
#include "eclipse/events/window_events.h"
#include "eclipse/events/application_events.h"
#include "eclipse/events/key_events.h"
#include "eclipse/events/mouse_events.h"
#include <glad/glad.h>

namespace eclipse {

	static bool GLFW_initialized = false;

	static void GLFW_error_callback(int error, const char* description) {
		EC_CORE_ERROR("GLFW Error (code: {0}) : {1}", error, description);
	}

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
			glfwSetErrorCallback(GLFW_error_callback);
			GLFW_initialized = true;
		}

		window_ = glfwCreateWindow(static_cast<int>(props.window_size.width), static_cast<int>(props.window_size.height), data_.props.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(window_);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		EC_CORE_ASSERT(status, "Failed to initialize Glad!")
		glfwSetWindowUserPointer(window_, &data_);
		set_v_sync(true);

		set_glfw_callbacks();
	}

	void WindowsWindow::shutdown() {
		glfwDestroyWindow(window_);
	}

	void WindowsWindow::set_glfw_callbacks() {
		glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.props.window_size.width = width;
			data.props.window_size.height = height;

			WindowResizeEvent event({ .width = static_cast<unsigned int>(width), .height = static_cast<unsigned int>(height) });
			data.event_callback(event);
		});

		glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowClosedEvent event;
			data.event_callback(event);
		});

		glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event({ .key_code = key, .repeat_count = 0 });
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(key);
				data.event_callback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event({ .key_code = key, .repeat_count = 1 });
				data.event_callback(event);
				break;
			}
			default: {
				EC_CORE_ERROR("Could not recognize key event.. Key: {0}, Scancode: {1}, Action: {2}, mods: {3}", key, scancode, action, mods);
				break;
			}
			}
		});

		glfwSetCharCallback(window_, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.event_callback(event);
		});

		glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(button);
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(button);
				data.event_callback(event);
				break;
			}
			default: {
				EC_CORE_ERROR("Could not recognize Mouse event.. Button: {0}, Action: {1}, Mods: {2}", button, action, mods);
				break;
			}
			}
		});

		glfwSetScrollCallback(window_, [](GLFWwindow* window, double x_offset, double y_offset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event({ .x = static_cast<float>(x_offset), .y = static_cast<float>(y_offset) });
			data.event_callback(event);
		});

		glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x_pose, double y_pose) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event({ .x = static_cast<float>(x_pose), .y = static_cast<float>(y_pose) });
			data.event_callback(event);
		});
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