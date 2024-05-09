#pragma once

#include "eclipse/renderer/renderer_api.h"
#include "eclipse/common_types/point_2d.h"
#include "eclipse/common_types/window_size.h"

#include <au.hh>

namespace eclipse {

class OpenGLRendererAPI : public RendererAPI {
public:
	void init() override;
	void set_viewport(const Point2D& origin, const WindowSize& size) override;
	void set_clear_color(const glm::vec4& color) override;
	void clear() override;
	void draw_indexed(const ref<VertexArray>& vertex_array) override;
	void draw_indexed(const ref<VertexArray>& vertex_array, uint32_t index_count) override;
	void draw_lines(const ref<VertexArray>& vertex_array, uint32_t vertex_count) override;
	void set_line_width(au::QuantityF<units::Pixels> width) override;
};

}  // namespace eclipse