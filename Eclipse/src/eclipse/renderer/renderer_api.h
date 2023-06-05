#pragma once

#include "eclipse/core.h"
#include "vertex_array.h"
#include <glm/glm.hpp>

namespace eclipse {

class ECLIPSE_API RendererAPI {
public:
	enum class API { none, open_gl };

	virtual void set_clear_color(const glm::vec4& color)            = 0;
	virtual void clear()                                            = 0;
	virtual void draw_indexed(const ref<VertexArray>& vertex_array) = 0;

	inline static API get_api() { return api_; }

private:
	static API api_;
};

}  // namespace eclipse