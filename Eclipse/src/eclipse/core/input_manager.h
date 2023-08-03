#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/point_2d.h"

namespace eclipse {

class ECLIPSE_API InputManager {
public:
	static bool is_key_pressed(int keycode);
	static bool is_mouse_button_pressed(int button);
	static Point2D get_mouse_pose();
	static float get_mouse_pose_x();
	static float get_mouse_pose_y();
};

}  // namespace eclipse