#pragma once

#include "eclipse/renderer/renderer_api.h"

namespace eclipse {

class OpenGLRendererAPI : public RendererAPI {
public:
	void set_clear_color(const glm::vec4& color) override;
	void clear() override;
	void draw_indexed(const std::shared_ptr<VertexArray>& vertex_array) override;
};

}  // namespace eclipse