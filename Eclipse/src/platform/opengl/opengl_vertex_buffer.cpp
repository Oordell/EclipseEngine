#include "ecpch.h"
#include "opengl_vertex_buffer.h"

#include <glad/glad.h>

namespace eclipse {

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size) {
	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() { glDeleteBuffers(1, &renderer_id_); }

void OpenGLVertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, renderer_id_); }

void OpenGLVertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

}  // namespace eclipse