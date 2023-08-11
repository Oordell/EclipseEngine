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
