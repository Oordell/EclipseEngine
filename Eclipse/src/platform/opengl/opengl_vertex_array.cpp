#include "ecpch.h"
#include "opengl_vertex_array.h"

#include <glad/glad.h>

namespace eclipse {

static GLenum shader_data_type_to_opengl_base_type(ShaderDataType type) {
	using enum ShaderDataType;
	switch (type) {
		case floatvec1:
			return GL_FLOAT;
		case floatvec2:
			return GL_FLOAT;
		case floatvec3:
			return GL_FLOAT;
		case floatvec4:
			return GL_FLOAT;
		case floatmat3:
			return GL_FLOAT;
		case floatmat4:
			return GL_FLOAT;
		case intvec1:
			return GL_INT;
		case intvec2:
			return GL_INT;
		case intvec3:
			return GL_INT;
		case intvec4:
			return GL_INT;
		case boolean:
			return GL_BOOL;
	}

	EC_CORE_ASSERT(false, "Unknown shader data type!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray() {
	EC_PROFILE_FUNCTION();

	glCreateVertexArrays(1, &renderer_id_);
}

OpenGLVertexArray::~OpenGLVertexArray() {
	EC_PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &renderer_id_);
}

void OpenGLVertexArray::bind() const {
	EC_PROFILE_FUNCTION();

	glBindVertexArray(renderer_id_);
}

void OpenGLVertexArray::unbind() const {
	EC_PROFILE_FUNCTION();

	glBindVertexArray(0);
}

void OpenGLVertexArray::add_vertex_buffer(const ref<VertexBuffer>& vertex_buffer) {
	EC_PROFILE_FUNCTION();

	EC_CORE_ASSERT(vertex_buffer->get_layout().get_elements().size(), "Vertex Buffer has no layout!");

	glBindVertexArray(renderer_id_);
	vertex_buffer->bind();

	uint32_t index     = 0;
	const auto& layout = vertex_buffer->get_layout();
	for (const auto& element : layout) {
		switch (element.type) {
			case ShaderDataType::floatvec1:
			case ShaderDataType::floatvec2:
			case ShaderDataType::floatvec3:
			case ShaderDataType::floatvec4:
			case ShaderDataType::intvec1:
			case ShaderDataType::intvec2:
			case ShaderDataType::intvec3:
			case ShaderDataType::intvec4:
			case ShaderDataType::boolean: {
				glEnableVertexAttribArray(vertex_buffer_index_);
				uint64_t offset = static_cast<uint64_t>(element.offset);
				glVertexAttribPointer(vertex_buffer_index_, element.get_component_count(),
				                      shader_data_type_to_opengl_base_type(element.type), element.normalized ? GL_TRUE : GL_FALSE,
				                      layout.get_stride(), reinterpret_cast<const void*>(offset));
				vertex_buffer_index_++;
				break;
			}
			case ShaderDataType::floatmat3:
			case ShaderDataType::floatmat4: {
				auto count = element.get_component_count();
				for (size_t i = 0; i < count; i++) {
					glEnableVertexAttribArray(vertex_buffer_index_);
					glVertexAttribPointer(vertex_buffer_index_, element.get_component_count(),
					                      shader_data_type_to_opengl_base_type(element.type), element.normalized ? GL_TRUE : GL_FALSE,
					                      layout.get_stride(),
					                      reinterpret_cast<const void*>(element.offset + sizeof(float) * count * i));
					glVertexAttribDivisor(vertex_buffer_index_, 1);
					vertex_buffer_index_++;
				}
				break;
			}
			default: {
				EC_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
	}

	vertex_buffers_.push_back(vertex_buffer);
}

void OpenGLVertexArray::set_index_buffer(const ref<IndexBuffer>& index_buffer) {
	EC_PROFILE_FUNCTION();

	glBindVertexArray(renderer_id_);
	index_buffer->bind();
	index_buffer_ = index_buffer;
}
}  // namespace eclipse