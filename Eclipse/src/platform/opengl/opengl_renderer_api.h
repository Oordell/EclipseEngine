#pragma once

#include "eclipse/renderer/renderer_api.h"
#include "eclipse/common_types/point_2d.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

class OpenGLRendererAPI : public RendererAPI {
public:
	void init() override;
	void set_viewport(const Point2D& origin, const WindowSize& size) override;
	void set_clear_color(const glm::vec4& color) override;
	void clear() override;
	void draw_indexed(const ref<VertexArray>& vertex_array) override;
	void draw_indexed(const ref<VertexArray>& vertex_array, uint32_t index_count) override;
};

}  // namespace eclipse