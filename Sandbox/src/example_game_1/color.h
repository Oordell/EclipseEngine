#pragma once

#include <glm/glm.hpp>

class ColorRGB {
public:
	float red   = 1.0F;
	float green = 1.0F;
	float blue  = 1.0F;
	float alpha = 1.0F;

	glm::vec4 get() const { return {red, green, blue, alpha}; }
};

class ColorHSV {
public:
	float hue = 1.0F;
	float sat = 1.0F;
	float val = 1.0F;

	glm::vec3 get() const { return {hue, sat, val}; }
};

static ColorRGB hsv_to_rgb(const ColorHSV& hsv) {
	int H    = (int) (hsv.hue * 360.0f);
	double S = hsv.sat;
	double V = hsv.val;

	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	} else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	} else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	} else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	} else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	} else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	return {.red   = static_cast<float>(Rs + m),
	        .green = static_cast<float>(Gs + m),
	        .blue  = static_cast<float>(Bs + m),
	        .alpha = 1.0F};
}
