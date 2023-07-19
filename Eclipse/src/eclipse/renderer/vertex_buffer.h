#pragma once

#include "eclipse/core/core.h"
#include "buffer_layout.h"

namespace eclipse {

class ECLIPSE_API VertexBuffer {
public:
	virtual ~VertexBuffer() = default;

	virtual void bind() const                                   = 0;
	virtual void unbind() const                                 = 0;
	virtual void set_layout(const BufferLayout& layout)         = 0;
	virtual const BufferLayout& get_layout() const              = 0;
	virtual void set_data(const void* data, uint32_t data_size) = 0;

	static ref<VertexBuffer> create(uint32_t size);
	static ref<VertexBuffer> create(float* vertices, uint32_t size);
};

}  // namespace eclipse