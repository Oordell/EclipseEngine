#include "ecpch.h"
#include "opengl_index_buffer.h"

#include <glad/glad.h>

namespace eclipse {

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : count_(count) {
	EC_PROFILE_FUNCTION();

	glCreateBuffers(1, &renderer_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_ * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() { 
	EC_PROFILE_FUNCTION();
	
	glDeleteBuffers(1, &renderer_id_);
}

void OpenGLIndexBuffer::bind() const {
	EC_PROFILE_FUNCTION();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_);
}

void OpenGLIndexBuffer::unbind() const { 
	EC_PROFILE_FUNCTION();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}  // namespace eclipse