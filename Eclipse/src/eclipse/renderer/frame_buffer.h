#pragma once

#include "eclipse/core/core.h"

namespace eclipse {

struct FrameBufferSpecification {
	int32_t width;
	int32_t height;
	int32_t samples        = 1;
	bool swap_chain_target = false;
};

class FrameBuffer {
public:
	static ref<FrameBuffer> create(const FrameBufferSpecification& specs);
	virtual ~FrameBuffer()                                            = default;
	virtual void bind()                                               = 0;
	virtual void unbind()                                             = 0;
	virtual const FrameBufferSpecification& get_specification() const = 0;
	virtual uint32_t get_color_attachment_renderer_id() const         = 0;

private:
};

}  // namespace eclipse