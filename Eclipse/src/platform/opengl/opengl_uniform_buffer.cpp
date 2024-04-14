#include "ecpch.h"
#include "opengl_uniform_buffer.h"

#include <glad/glad.h>

namespace eclipse {
OpenGLUniformBuffer::OpenGLUniformBuffer(const UniformBufferRecipe& args) {
	glCreateBuffers(1, &renderer_id_);
	glNamedBufferData(renderer_id_, args.size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, args.binding, renderer_id_);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() { glDeleteBuffers(1, &renderer_id_); }

void OpenGLUniformBuffer::set_data(const void* data, const UniformBufferDataDescription& details) {
	glNamedBufferSubData(renderer_id_, details.offset, details.size, data);
}
}  // namespace eclipse