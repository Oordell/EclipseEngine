#pragma once

#include "eclipse/renderer/vertex_array.h"

namespace eclipse {

class ECLIPSE_API OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	~OpenGLVertexArray() override;
	void bind() const override;
	void unbind() const override;
	void add_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertex_buffer) override;
	void set_index_buffer(const std::shared_ptr<IndexBuffer>& index_buffer) override;

	const std::vector<std::shared_ptr<VertexBuffer>>& get_vertex_buffers() const override { return vertex_buffers_; }

	const std::shared_ptr<IndexBuffer>& get_index_buffer() const override { return index_buffer_; }

private:
	uint32_t renderer_id_ {};
	std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
	std::shared_ptr<IndexBuffer> index_buffer_;
};

}  // namespace eclipse