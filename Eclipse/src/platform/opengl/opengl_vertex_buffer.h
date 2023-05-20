#pragma once

#include "eclipse/renderer/vertex_buffer.h"

namespace eclipse {

	class ECLIPSE_API OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		void bind() const override;
		void unbind() const override;

	private:
		uint32_t renderer_id_ = 0;
	};

}