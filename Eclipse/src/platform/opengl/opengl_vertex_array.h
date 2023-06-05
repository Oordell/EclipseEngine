#pragma once

#include "eclipse/renderer/vertex_array.h"

namespace eclipse {

class ECLIPSE_API OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	~OpenGLVertexArray() override;
	void bind() const override;
	void unbind() const override;
	void add_vertex_buffer(const ref<VertexBuffer>& vertex_buffer) override;
	void set_index_buffer(const ref<IndexBuffer>& index_buffer) override;

	const std::vector<ref<VertexBuffer>>& get_vertex_buffers() const override { return vertex_buffers_; }

	const ref<IndexBuffer>& get_index_buffer() const override { return index_buffer_; }

private:
	uint32_t renderer_id_ {};
	std::vector<ref<VertexBuffer>> vertex_buffers_;
	ref<IndexBuffer> index_buffer_;
};

}  // namespace eclipse