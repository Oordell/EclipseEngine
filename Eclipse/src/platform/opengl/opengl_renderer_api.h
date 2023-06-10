#pragma once

#include "eclipse/renderer/renderer_api.h"

namespace eclipse {

class OpenGLRendererAPI : public RendererAPI {
public:
	void init() override;
	void set_clear_color(const glm::vec4& color) override;
	void clear() override;
	void draw_indexed(const ref<VertexArray>& vertex_array) override;
};

}  // namespace eclipse