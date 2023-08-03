#pragma once

namespace eclipse {

enum class MouseCode : uint16_t {
	none          = 0xffff,
	button0       = 0,
	button1       = 1,
	button2       = 2,
	button3       = 3,
	button4       = 4,
	button5       = 5,
	button6       = 6,
	button7       = 7,
	button_last   = button7,
	button_left   = button0,
	button_right  = button1,
	button_middle = button2,
};

inline std::ostream& operator<<(std::ostream& os, MouseCode mouse_code) {
	os << static_cast<int32_t>(mouse_code);
	return os;
}
}  // namespace eclipse

// Taken from glfw3.h
#define EC_MOUSE_BUTTON_1      ::eclipse::MouseCode::button0
#define EC_MOUSE_BUTTON_2      ::eclipse::MouseCode::button1
#define EC_MOUSE_BUTTON_3      ::eclipse::MouseCode::button2
#define EC_MOUSE_BUTTON_4      ::eclipse::MouseCode::button3
#define EC_MOUSE_BUTTON_5      ::eclipse::MouseCode::button4
#define EC_MOUSE_BUTTON_6      ::eclipse::MouseCode::button5
#define EC_MOUSE_BUTTON_7      ::eclipse::MouseCode::button6
#define EC_MOUSE_BUTTON_8      ::eclipse::MouseCode::button7
#define EC_MOUSE_BUTTON_LAST   ::eclipse::MouseCode::button_last
#define EC_MOUSE_BUTTON_LEFT   ::eclipse::MouseCode::button_left
#define EC_MOUSE_BUTTON_RIGHT  ::eclipse::MouseCode::button_right
#define EC_MOUSE_BUTTON_MIDDLE ::eclipse::MouseCode::button_middle