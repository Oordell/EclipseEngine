#include "ecpch.h"
#include "opengl_renderer_api.h"

#include <glad/glad.h>

namespace eclipse {

void opengl_message_callback(unsigned int /*source*/, unsigned int /*type*/, unsigned int /*id*/, unsigned int severity,
                             int /*length*/, const char* message, const void* /*user_param*/) {
	switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: {
			EC_CORE_FATAL(message);
			return;
		}
		case GL_DEBUG_SEVERITY_MEDIUM: {
			EC_CORE_WARN(message);
			return;
		}
		case GL_DEBUG_SEVERITY_LOW: {
			EC_CORE_INFO(message);
			return;
		}
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			EC_CORE_TRACE(message);
			return;
		}
	}

	EC_CORE_ASSERT(false, "Unknown severity level!");
}

void OpenGLRendererAPI::init() {
	EC_PROFILE_FUNCTION();

#ifdef ECLIPSE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_message_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
}

void OpenGLRendererAPI::set_viewport(const Point2D& origin, const WindowSize& size) {
	glViewport(static_cast<GLint>(origin.x.in(units::pixels)), static_cast<GLint>(origin.y.in(units::pixels)),
	           size.width.in(units::pixels), size.height.in(units::pixels));
}

void OpenGLRendererAPI::set_clear_color(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }

void OpenGLRendererAPI::clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void OpenGLRendererAPI::draw_indexed(const ref<VertexArray>& vertex_array) {
	draw_indexed(vertex_array, vertex_array->get_index_buffer()->get_count());
}

void OpenGLRendererAPI::draw_indexed(const ref<VertexArray>& vertex_array, uint32_t index_count) {
	vertex_array->bind();
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLRendererAPI::draw_lines(const ref<VertexArray>& vertex_array, uint32_t vertex_count) {
	vertex_array->bind();
	glDrawArrays(GL_LINES, 0, vertex_count);
}

void OpenGLRendererAPI::set_line_width(au::QuantityF<units::Pixels> width) { glLineWidth(width.in(units::pixels)); }

}  // namespace eclipse