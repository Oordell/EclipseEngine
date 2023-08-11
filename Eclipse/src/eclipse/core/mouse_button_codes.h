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
