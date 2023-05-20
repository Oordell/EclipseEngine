#pragma once

#include "eclipse/core.h"

namespace eclipse {

	class ECLIPSE_API VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		static VertexBuffer* create(float* vertices, uint32_t size);
	};

}