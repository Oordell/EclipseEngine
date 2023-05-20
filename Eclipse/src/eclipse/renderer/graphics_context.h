#pragma once

#include "eclipse/core.h"

namespace eclipse {

	class ECLIPSE_API GraphicsContext {
	public:
		virtual void init() = 0;
		virtual void swap_buffers() = 0;
	};

}