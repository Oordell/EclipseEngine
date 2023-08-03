#pragma once

#include "eclipse/renderer/frame_buffer.h"

namespace eclipse {

class OpenGLFrameBuffer : public FrameBuffer {
public:
	OpenGLFrameBuffer(const FrameBufferSpecification& specs);
	~OpenGLFrameBuffer() override;
	void bind() override;
	void unbind() override;
	void resize(const WindowSize& size);

	const FrameBufferSpecification& get_specification() const override { return specifications_; }

	uint32_t get_color_attachment_renderer_id() const override { return color_attachment_; }

	void invalidate();

private:
	void reset();

	uint32_t renderer_id_      = 0;
	uint32_t color_attachment_ = 0;
	uint32_t depth_attachment_ = 0;
	FrameBufferSpecification specifications_;
};

}  // namespace eclipse