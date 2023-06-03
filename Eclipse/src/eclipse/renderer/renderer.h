#pragma once

#include "eclipse/core.h"
#include "render_command.h"

namespace eclipse {

class ECLIPSE_API Renderer {
public:
	static void begin_scene();

	static void end_scene();

	static void submit(const std::shared_ptr<VertexArray>& vertex_array);

	inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }
};
}  // namespace eclipse