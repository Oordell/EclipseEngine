#pragma once

#include "eclipse/core.h"
#include "eclipse/common_types/point_2d.h"

namespace eclipse {

class ECLIPSE_API InputManager {
public:
	inline static bool is_key_pressed(int keycode) { return instance_->is_key_pressed_impl(keycode); }

	inline static bool is_mouse_button_pressed(int button) { return instance_->is_mouse_button_pressed_impl(button); }

	inline static Point2D get_mouse_pose() { return instance_->get_mouse_pose_impl(); }

	inline static float get_mouse_pose_x() { return instance_->get_mouse_pose_x_impl(); }

	inline static float get_mouse_pose_y() { return instance_->get_mouse_pose_y_impl(); }

protected:
	virtual bool is_key_pressed_impl(int keycode) = 0;

	virtual bool is_mouse_button_pressed_impl(int button) = 0;
	virtual Point2D get_mouse_pose_impl()                 = 0;
	virtual float get_mouse_pose_x_impl()                 = 0;
	virtual float get_mouse_pose_y_impl()                 = 0;

private:
	static InputManager* instance_;
};

}  // namespace eclipse