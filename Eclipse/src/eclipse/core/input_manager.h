#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/point_2d.h"
#include "eclipse/core/key_codes.h"
#include "eclipse/core/mouse_button_codes.h"

namespace eclipse {

class ECLIPSE_API InputManager {
public:
	static bool is_key_pressed(KeyCode keycode);
	static bool is_mouse_button_pressed(MouseCode button);
	static Point2D get_mouse_pose();
	static au::QuantityF<units::Pixels> get_mouse_pose_x();
	static au::QuantityF<units::Pixels> get_mouse_pose_y();
};

}  // namespace eclipse