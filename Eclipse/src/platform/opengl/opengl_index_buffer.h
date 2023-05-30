#pragma once

#include "eclipse/renderer/index_buffer.h"

namespace eclipse {

class ECLIPSE_API OpenGLIndexBuffer : public IndexBuffer {
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
	~OpenGLIndexBuffer();

	void bind() const override;
	void unbind() const override;

	uint32_t get_count() const override { return count_; }

private:
	uint32_t renderer_id_ = 0;
	uint32_t count_;
};

}  // namespace eclipse