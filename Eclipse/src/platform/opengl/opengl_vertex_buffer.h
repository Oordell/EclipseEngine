#pragma once

#include "eclipse/renderer/vertex_buffer.h"

namespace eclipse {

class ECLIPSE_API OpenGLVertexBuffer : public VertexBuffer {
public:
	OpenGLVertexBuffer(uint32_t size);
	OpenGLVertexBuffer(float* vertices, uint32_t size);
	~OpenGLVertexBuffer();

	void bind() const override;
	void unbind() const override;

	void set_layout(const BufferLayout& layout) override { layout_ = layout; }

	const BufferLayout& get_layout() const override { return layout_; }

	void set_data(const void* data, uint32_t data_size) override;

private:
	uint32_t renderer_id_ = 0;
	BufferLayout layout_;
};

}  // namespace eclipse