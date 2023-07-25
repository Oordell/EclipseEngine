#include "ecpch.h"
#include "opengl_vertex_buffer.h"

#include <glad/glad.h>

namespace eclipse {
OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) {
	EC_PROFILE_FUNCTION();

	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
	EC_PROFILE_FUNCTION();

	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
	EC_PROFILE_FUNCTION();

	glDeleteBuffers(1, &renderer_id_);
}

void OpenGLVertexBuffer::bind() const {
	EC_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
}

void OpenGLVertexBuffer::unbind() const {
	EC_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::set_data(const void* data, uint32_t data_size) {
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);
}

}  // namespace eclipse