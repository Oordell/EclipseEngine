#include "ecpch.h"
#include "windows_window.h"
#include "eclipse/core/core.h"
#include "eclipse/core/key_codes.h"
#include "eclipse/core/mouse_button_codes.h"
#include "eclipse/events/window_events.h"
#include "eclipse/events/application_events.h"
#include "eclipse/events/key_events.h"
#include "eclipse/events/mouse_events.h"
#include "eclipse/renderer/renderer.h"
#include "platform/opengl/opengl_context.h"

namespace eclipse {

static uint16_t GLFW_initialized = 0;

static void GLFW_error_callback(int error, const char* description) {
	EC_CORE_ERROR("GLFW Error (code: {0}) : {1}", error, description);
}

WindowsWindow::WindowsWindow(const WindowProps& props) {
	EC_PROFILE_FUNCTION();
	init(props);
}

WindowsWindow::~WindowsWindow() {
	EC_PROFILE_FUNCTION();
	shutdown();
}

void WindowsWindow::init(const WindowProps& props) {
	EC_PROFILE_FUNCTION();
	data_.props = props;

	EC_CORE_INFO("Creating window \"{0}\" of size ({1}, {2})", props.title, props.window_size.width,
	             props.window_size.height);

	if (GLFW_initialized == 0) {
		EC_PROFILE_SCOPE("WindowsWindow::init(const WindowProps&)::glfwInit()");
		EC_CORE_INFO("Initializing GLFW");
		auto success = glfwInit();
		EC_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFW_error_callback);
	}

	{
		EC_PROFILE_SCOPE("WindowsWindow::init(const WindowProps&)::glfwCreateWindow()");

#if defined(ECLIPSE_DEBUG)
		if (Renderer::get_api() == RendererAPI::API::open_gl) {
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		}
#endif

		window_ = glfwCreateWindow(static_cast<int>(props.window_size.width), static_cast<int>(props.window_size.height),
		                           data_.props.title.c_str(), nullptr, nullptr);
	}
	++GLFW_initialized;
	context_ = GraphicsContext::create(window_);
	context_->init();
	glfwSetWindowUserPointer(window_, &data_);
	set_v_sync(true);

	set_glfw_callbacks();
}

void WindowsWindow::shutdown() {
	EC_PROFILE_FUNCTION();
	glfwDestroyWindow(window_);
	--GLFW_initialized;
	if (GLFW_initialized == 0) {
		EC_CORE_INFO("Terminating GLFW");
		glfwTerminate();
	}
}

void WindowsWindow::set_glfw_callbacks() {
	glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
		WindowData& data              = *(WindowData*) glfwGetWindowUserPointer(window);
		data.props.window_size.width  = width;
		data.props.window_size.height = height;

		WindowResizeEvent event({.width = static_cast<unsigned int>(width), .height = static_cast<unsigned int>(height)});
		data.event_callback(event);
	});

	glfwSetWindowCloseCallback(window_, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		WindowClosedEvent event;
		data.event_callback(event);
	});

	glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		switch (action) {
			case GLFW_PRESS: {
				KeyPressedEvent event({.key_code = static_cast<KeyCode>(key), .repeat_count = 0});
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(static_cast<KeyCode>(key));
				data.event_callback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event({.key_code = static_cast<KeyCode>(key), .repeat_count = 1});
				data.event_callback(event);
				break;
			}
			default: {
				EC_CORE_ERROR("Could not recognize key event.. Key: {0}, Scancode: {1}, Action: {2}, mods: {3}", key, scancode,
				              action, mods);
				break;
			}
		}
	});

	glfwSetCharCallback(window_, [](GLFWwindow* window, unsigned int keycode) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		KeyTypedEvent event(static_cast<KeyCode>(keycode));
		data.event_callback(event);
	});

	glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		switch (action) {
			case GLFW_PRESS: {
				MouseButtonPressedEvent event(static_cast<MouseCode>(button));
				data.event_callback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
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
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		MouseScrolledEvent event({.x = static_cast<float>(x_offset), .y = static_cast<float>(y_offset)});
		data.event_callback(event);
	});

	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x_pose, double y_pose) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

		MouseMovedEvent event({.x = static_cast<float>(x_pose), .y = static_cast<float>(y_pose)});
		data.event_callback(event);
	});
}

void WindowsWindow::on_update() {
	EC_PROFILE_FUNCTION();
	glfwPollEvents();
	context_->swap_buffers();
}

void WindowsWindow::set_v_sync(bool enabled) {
	EC_PROFILE_FUNCTION();
	int input = enabled ? 1 : 0;
	glfwSwapInterval(input);
	data_.v_sync = enabled;
}

bool WindowsWindow::is_v_sync() const { return data_.v_sync; }
}  // namespace eclipse