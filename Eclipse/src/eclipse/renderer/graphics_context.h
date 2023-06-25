#pragma once

#include "eclipse/core/core.h"

namespace eclipse {

class ECLIPSE_API GraphicsContext {
public:
	virtual void init()         = 0;
	virtual void swap_buffers() = 0;
	static scope<GraphicsContext> create(void* window);
};

}  // namespace eclipse