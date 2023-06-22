#pragma once

#include "eclipse/core/input_manager.h"

namespace eclipse {

class WindowsInput : public InputManager {
protected:
	bool is_key_pressed_impl(int keycode) override;

	bool is_mouse_button_pressed_impl(int button) override;
	Point2D get_mouse_pose_impl() override;
	float get_mouse_pose_x_impl() override;
	float get_mouse_pose_y_impl() override;
};

}  // namespace eclipse