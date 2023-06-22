#pragma once

#include "eclipse/core/core.h"
#include "renderer_api.h"
#include "eclipse/common_types/point_2d.h"

namespace eclipse {

class ECLIPSE_API RenderCommand {
public:
	inline static void init() { renderer_api_->init(); }

	inline static void set_viewport(const Point2D& origin, const WindowSize& size) {
		renderer_api_->set_viewport(origin, size);
	}

	inline static void set_clear_color(const glm::vec4& color) { renderer_api_->set_clear_color(color); }

	inline static void clear() { renderer_api_->clear(); }

	inline static void draw_indexed(const ref<VertexArray>& vertex_array) { renderer_api_->draw_indexed(vertex_array); }

private:
	static RendererAPI* renderer_api_;
};

}  // namespace eclipse