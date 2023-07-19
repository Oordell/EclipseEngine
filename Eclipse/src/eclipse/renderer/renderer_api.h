#pragma once

#include "eclipse/core/core.h"
#include "vertex_array.h"
#include <glm/glm.hpp>
#include "eclipse/common_types/point_2d.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

class ECLIPSE_API RendererAPI {
public:
	enum class API { none, open_gl };

	virtual void init()                                                                   = 0;
	virtual void set_viewport(const Point2D& origin, const WindowSize& size)              = 0;
	virtual void set_clear_color(const glm::vec4& color)                                  = 0;
	virtual void clear()                                                                  = 0;
	virtual void draw_indexed(const ref<VertexArray>& vertex_array)                       = 0;
	virtual void draw_indexed(const ref<VertexArray>& vertex_array, uint32_t index_count) = 0;

	inline static API get_api() { return api_; }

	static scope<RendererAPI> create();

private:
	static API api_;
};

}  // namespace eclipse