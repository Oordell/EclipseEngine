#pragma once

namespace eclipse {

enum class KeyCode : uint16_t {
	none = 0,

	space      = 32,
	apostrophe = 39, /* ' */
	comma      = 44, /* , */
	minus      = 45, /* - */
	period     = 46, /* . */
	slash      = 47, /* / */

	d0 = 48,
	d1 = 49,
	d2 = 50,
	d3 = 51,
	d4 = 52,
	d5 = 53,
	d6 = 54,
	d7 = 55,
	d8 = 56,
	d9 = 57,

	semicolon = 59, /* ; */
	equal     = 61, /* = */

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90,

	left_bracket  = 91, /* [ */
	backslash     = 92, /* \ */
	right_bracket = 93, /* ] */
	grave_accent  = 96, /* ` */

	world_1 = 161, /* non-US #1 */
	world_2 = 162, /* non-US #2 */

	escape       = 256,
	enter        = 257,
	tab          = 258,
	backspace    = 259,
	insert       = 260,
	delete_btn   = 261,
	right        = 262,
	left         = 263,
	down         = 264,
	up           = 265,
	page_up      = 266,
	page_down    = 267,
	home         = 268,
	end          = 269,
	caps_lock    = 280,
	scroll_lock  = 281,
	num_lock     = 282,
	print_screen = 283,
	pause        = 284,
	F1           = 290,
	F2           = 291,
	F3           = 292,
	F4           = 293,
	F5           = 294,
	F6           = 295,
	F7           = 296,
	F8           = 297,
	F9           = 298,
	F10          = 299,
	F11          = 300,
	F12          = 301,
	F13          = 302,
	F14          = 303,
	F15          = 304,
	F16          = 305,
	F17          = 306,
	F18          = 307,
	F19          = 308,
	F20          = 309,
	F21          = 310,
	F22          = 311,
	F23          = 312,
	F24          = 313,
	F25          = 314,

	//	KeyPads
	kp_0        = 320,
	kp_1        = 321,
	kp_2        = 322,
	kp_3        = 323,
	kp_4        = 324,
	kp_5        = 325,
	kp_6        = 326,
	kp_7        = 327,
	kp_8        = 328,
	kp_9        = 329,
	kp_decimal  = 330,
	kp_divide   = 331,
	kp_multiply = 332,
	kp_subtract = 333,
	kp_add      = 334,
	kp_enter    = 335,
	kp_equal    = 336,

	left_shift    = 340,
	left_control  = 341,
	left_alt      = 342,
	left_super    = 343,
	right_shift   = 344,
	right_control = 345,
	right_alt     = 346,
	right_super   = 347,
	menu          = 348,
};

inline std::ostream& operator<<(std::ostream& os, KeyCode key_code) {
	os << static_cast<int32_t>(key_code);
	return os;
}

}  // namespace eclipse

// Taken from glfw3.h
#define EC_KEY_SPACE         ::eclipse::KeyCode::space
#define EC_KEY_APOSTROPHE    ::eclipse::KeyCode::apostrophe
#define EC_KEY_COMMA         ::eclipse::KeyCode::comma
#define EC_KEY_MINUS         ::eclipse::KeyCode::minus
#define EC_KEY_PERIOD        ::eclipse::KeyCode::period
#define EC_KEY_SLASH         ::eclipse::KeyCode::slash
#define EC_KEY_0             ::eclipse::KeyCode::d0
#define EC_KEY_1             ::eclipse::KeyCode::d1
#define EC_KEY_2             ::eclipse::KeyCode::d2
#define EC_KEY_3             ::eclipse::KeyCode::d3
#define EC_KEY_4             ::eclipse::KeyCode::d4
#define EC_KEY_5             ::eclipse::KeyCode::d5
#define EC_KEY_6             ::eclipse::KeyCode::d6
#define EC_KEY_7             ::eclipse::KeyCode::d7
#define EC_KEY_8             ::eclipse::KeyCode::d8
#define EC_KEY_9             ::eclipse::KeyCode::d9
#define EC_KEY_SEMICOLON     ::eclipse::KeyCode::semicolon
#define EC_KEY_EQUAL         ::eclipse::KeyCode::equal
#define EC_KEY_A             ::eclipse::KeyCode::A
#define EC_KEY_B             ::eclipse::KeyCode::B
#define EC_KEY_C             ::eclipse::KeyCode::C
#define EC_KEY_D             ::eclipse::KeyCode::D
#define EC_KEY_E             ::eclipse::KeyCode::E
#define EC_KEY_F             ::eclipse::KeyCode::F
#define EC_KEY_G             ::eclipse::KeyCode::G
#define EC_KEY_H             ::eclipse::KeyCode::H
#define EC_KEY_I             ::eclipse::KeyCode::I
#define EC_KEY_J             ::eclipse::KeyCode::J
#define EC_KEY_K             ::eclipse::KeyCode::K
#define EC_KEY_L             ::eclipse::KeyCode::L
#define EC_KEY_M             ::eclipse::KeyCode::M
#define EC_KEY_N             ::eclipse::KeyCode::N
#define EC_KEY_O             ::eclipse::KeyCode::O
#define EC_KEY_P             ::eclipse::KeyCode::P
#define EC_KEY_Q             ::eclipse::KeyCode::Q
#define EC_KEY_R             ::eclipse::KeyCode::R
#define EC_KEY_S             ::eclipse::KeyCode::S
#define EC_KEY_T             ::eclipse::KeyCode::T
#define EC_KEY_U             ::eclipse::KeyCode::U
#define EC_KEY_V             ::eclipse::KeyCode::V
#define EC_KEY_W             ::eclipse::KeyCode::W
#define EC_KEY_X             ::eclipse::KeyCode::X
#define EC_KEY_Y             ::eclipse::KeyCode::Y
#define EC_KEY_Z             ::eclipse::KeyCode::Z
#define EC_KEY_LEFT_BRACKET  ::eclipse::KeyCode::left_bracket
#define EC_KEY_BACKSLASH     ::eclipse::KeyCode::backslash
#define EC_KEY_RIGHT_BRACKET ::eclipse::KeyCode::right_bracket
#define EC_KEY_GRAVE_ACCENT  ::eclipse::KeyCode::grave_accent
#define EC_KEY_WORLD_1       ::eclipse::KeyCode::world_1
#define EC_KEY_WORLD_2       ::eclipse::KeyCode::world_2

/* Function keys */
#define EC_KEY_ESCAPE        ::eclipse::KeyCode::escape
#define EC_KEY_ENTER         ::eclipse::KeyCode::enter
#define EC_KEY_TAB           ::eclipse::KeyCode::tab
#define EC_KEY_BACKSPACE     ::eclipse::KeyCode::backspace
#define EC_KEY_INSERT        ::eclipse::KeyCode::insert
#define EC_KEY_DELETE        ::eclipse::KeyCode::delete_btn
#define EC_KEY_RIGHT         ::eclipse::KeyCode::right
#define EC_KEY_LEFT          ::eclipse::KeyCode::left
#define EC_KEY_DOWN          ::eclipse::KeyCode::down
#define EC_KEY_UP            ::eclipse::KeyCode::up
#define EC_KEY_PAGE_UP       ::eclipse::KeyCode::page_up
#define EC_KEY_PAGE_DOWN     ::eclipse::KeyCode::page_down
#define EC_KEY_HOME          ::eclipse::KeyCode::home
#define EC_KEY_END           ::eclipse::KeyCode::end
#define EC_KEY_CAPS_LOCK     ::eclipse::KeyCode::caps_lock
#define EC_KEY_SCROLL_LOCK   ::eclipse::KeyCode::scroll_lock
#define EC_KEY_NUM_LOCK      ::eclipse::KeyCode::num_lock
#define EC_KEY_PRINT_SCREEN  ::eclipse::KeyCode::print_screen
#define EC_KEY_PAUSE         ::eclipse::KeyCode::pause
#define EC_KEY_F1            ::eclipse::KeyCode::F1
#define EC_KEY_F2            ::eclipse::KeyCode::F2
#define EC_KEY_F3            ::eclipse::KeyCode::F3
#define EC_KEY_F4            ::eclipse::KeyCode::F4
#define EC_KEY_F5            ::eclipse::KeyCode::F5
#define EC_KEY_F6            ::eclipse::KeyCode::F6
#define EC_KEY_F7            ::eclipse::KeyCode::F7
#define EC_KEY_F8            ::eclipse::KeyCode::F8
#define EC_KEY_F9            ::eclipse::KeyCode::F9
#define EC_KEY_F10           ::eclipse::KeyCode::F10
#define EC_KEY_F11           ::eclipse::KeyCode::F11
#define EC_KEY_F12           ::eclipse::KeyCode::F12
#define EC_KEY_F13           ::eclipse::KeyCode::F13
#define EC_KEY_F14           ::eclipse::KeyCode::F14
#define EC_KEY_F15           ::eclipse::KeyCode::F15
#define EC_KEY_F16           ::eclipse::KeyCode::F16
#define EC_KEY_F17           ::eclipse::KeyCode::F17
#define EC_KEY_F18           ::eclipse::KeyCode::F18
#define EC_KEY_F19           ::eclipse::KeyCode::F19
#define EC_KEY_F20           ::eclipse::KeyCode::F20
#define EC_KEY_F21           ::eclipse::KeyCode::F21
#define EC_KEY_F22           ::eclipse::KeyCode::F22
#define EC_KEY_F23           ::eclipse::KeyCode::F23
#define EC_KEY_F24           ::eclipse::KeyCode::F24
#define EC_KEY_F25           ::eclipse::KeyCode::F25
#define EC_KEY_KP_0          ::eclipse::KeyCode::kp_0
#define EC_KEY_KP_1          ::eclipse::KeyCode::kp_1
#define EC_KEY_KP_2          ::eclipse::KeyCode::kp_2
#define EC_KEY_KP_3          ::eclipse::KeyCode::kp_3
#define EC_KEY_KP_4          ::eclipse::KeyCode::kp_4
#define EC_KEY_KP_5          ::eclipse::KeyCode::kp_5
#define EC_KEY_KP_6          ::eclipse::KeyCode::kp_6
#define EC_KEY_KP_7          ::eclipse::KeyCode::kp_7
#define EC_KEY_KP_8          ::eclipse::KeyCode::kp_8
#define EC_KEY_KP_9          ::eclipse::KeyCode::kp_9
#define EC_KEY_KP_DECIMAL    ::eclipse::KeyCode::kp_decimal
#define EC_KEY_KP_DIVIDE     ::eclipse::KeyCode::kp_divide
#define EC_KEY_KP_MULTIPLY   ::eclipse::KeyCode::kp_multiply
#define EC_KEY_KP_SUBTRACT   ::eclipse::KeyCode::kp_subtract
#define EC_KEY_KP_ADD        ::eclipse::KeyCode::kp_add
#define EC_KEY_KP_ENTER      ::eclipse::KeyCode::kp_enter
#define EC_KEY_KP_EQUAL      ::eclipse::KeyCode::kp_equal
#define EC_KEY_LEFT_SHIFT    ::eclipse::KeyCode::left_shift
#define EC_KEY_LEFT_CONTROL  ::eclipse::KeyCode::left_control
#define EC_KEY_LEFT_ALT      ::eclipse::KeyCode::left_alt
#define EC_KEY_LEFT_SUPER    ::eclipse::KeyCode::left_super
#define EC_KEY_RIGHT_SHIFT   ::eclipse::KeyCode::right_shift
#define EC_KEY_RIGHT_CONTROL ::eclipse::KeyCode::right_control
#define EC_KEY_RIGHT_ALT     ::eclipse::KeyCode::right_alt
#define EC_KEY_RIGHT_SUPER   ::eclipse::KeyCode::right_super
#define EC_KEY_MENU          ::eclipse::KeyCode::menu
