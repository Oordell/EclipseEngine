#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/vertex_buffer.h"
#include "eclipse/renderer/index_buffer.h"

namespace eclipse {

class ECLIPSE_API VertexArray {
public:
	virtual ~VertexArray()                                                   = default;
	virtual void bind() const                                                = 0;
	virtual void unbind() const                                              = 0;
	virtual void add_vertex_buffer(const ref<VertexBuffer>& vertex_buffer)   = 0;
	virtual void set_index_buffer(const ref<IndexBuffer>& index_buffer)      = 0;
	virtual const std::vector<ref<VertexBuffer>>& get_vertex_buffers() const = 0;
	virtual const ref<IndexBuffer>& get_index_buffer() const                 = 0;
	static ref<VertexArray> create();
};

}  // namespace eclipse