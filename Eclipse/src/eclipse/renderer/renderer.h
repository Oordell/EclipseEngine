#pragma once

#include "eclipse/core.h"

namespace eclipse {

	enum class RendererAPI {
		none,
		open_gl
	};

	class ECLIPSE_API Renderer {
	public:
		inline static RendererAPI get_api() { return renderer_api_; }
		inline static void set_api(const RendererAPI& api) { renderer_api_ = api; }

	private:
		static RendererAPI renderer_api_;
	};
}