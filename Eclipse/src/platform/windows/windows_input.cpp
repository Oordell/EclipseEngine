#include "ecpch.h"
#include "eclipse/core/input_manager.h"
#include "eclipse/core/application.h"

#include <GLFW/glfw3.h>

namespace eclipse {

bool InputManager::is_key_pressed(KeyCode keycode) {
	auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
	auto state   = glfwGetKey(window, static_cast<int32_t>(keycode));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool InputManager::is_mouse_button_pressed(MouseCode button) {
	auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
	auto state   = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

Point2D InputManager::get_mouse_pose() {
	auto* window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
	double x_pose {};
	double y_pose {};
	glfwGetCursorPos(window, &x_pose, &y_pose);

	return {.x = units::pixels(static_cast<float>(x_pose)), .y = units::pixels(static_cast<float>(y_pose))};
}

au::QuantityF<units::Pixels> InputManager::get_mouse_pose_x() { return get_mouse_pose().x; }

au::QuantityF<units::Pixels> InputManager::get_mouse_pose_y() { return get_mouse_pose().y; }

}  // namespace eclipse