#include "ecpch.h"
#include "opengl_renderer_api.h"

#include <glad/glad.h>

namespace eclipse {

void OpenGLRendererAPI::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRendererAPI::set_viewport(const Point2D& origin, const WindowSize& size) {
	glViewport(origin.x, origin.y, size.width, size.height);
}

void OpenGLRendererAPI::set_clear_color(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

void OpenGLRendererAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void OpenGLRendererAPI::draw_indexed(const ref<VertexArray>& vertex_array) {
	glDrawElements(GL_TRIANGLES, vertex_array->get_index_buffer()->get_count(), GL_UNSIGNED_INT, nullptr);
}

}  // namespace eclipse