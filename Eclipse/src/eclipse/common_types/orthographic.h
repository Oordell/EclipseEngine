#pragma once

#include "eclipse/core.h"

namespace eclipse {

struct ECLIPSE_API OrthograficLimits {
	float left {0.0F};
	float right {0.0F};
	float bottom {0.0F};
	float top {0.0F};
	float near_plane {-1.0F};
	float far_plane {1.0F};
};

}  // namespace eclipse