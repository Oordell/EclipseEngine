#include "ecpch.h"
#include "windows_input.h"
#include "eclipse/application.h"

#include <GLFW/glfw3.h>

namespace eclipse {

	InputManager* InputManager::instance_ = new WindowsInput();

	bool WindowsInput::is_key_pressed_impl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::is_mouse_button_pressed_impl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetKey(window, button);
		return state == GLFW_PRESS;
	}

	Point2D WindowsInput::get_mouse_pose_impl() {
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		double x_pose {};
		double y_pose {};
		glfwGetCursorPos(window, &x_pose, &y_pose);

		return {.x = static_cast<float>(x_pose), .y = static_cast<float>(y_pose)};
	}

	float WindowsInput::get_mouse_pose_x_impl() {
		return get_mouse_pose_impl().x;
	}

	float WindowsInput::get_mouse_pose_y_impl() {
		return get_mouse_pose_impl().y;
	}

}