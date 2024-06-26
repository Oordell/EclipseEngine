#pragma once

#include "eclipse/core/core.h"
#include "renderer_api.h"
#include "eclipse/common_types/point_2d.h"

#include <au.hh>

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

	inline static void draw_indexed(const ref<VertexArray>& vertex_array, uint32_t index_count) {
		renderer_api_->draw_indexed(vertex_array, index_count);
	}

	inline static void draw_lines(const ref<VertexArray>& vertex_array, uint32_t vertex_count) {
		renderer_api_->draw_lines(vertex_array, vertex_count);
	}

	inline static void set_line_width(au::QuantityF<units::Pixels> width) { renderer_api_->set_line_width(width); }

private:
	static scope<RendererAPI> renderer_api_;
};

}  // namespace eclipse