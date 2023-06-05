#pragma once

#include "eclipse/core.h"
#include "renderer_api.h"

namespace eclipse {

class ECLIPSE_API RenderCommand {
public:
	inline static void set_clear_color(const glm::vec4& color) { renderer_api_->set_clear_color(color); }

	inline static void clear() { renderer_api_->clear(); }

	inline static void draw_indexed(const ref<VertexArray>& vertex_array) { renderer_api_->draw_indexed(vertex_array); }

private:
	static RendererAPI* renderer_api_;
};

}  // namespace eclipse