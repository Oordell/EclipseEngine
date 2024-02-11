#pragma once

#include "eclipse/renderer/frame_buffer.h"

namespace eclipse {

class OpenGLFrameBuffer : public FrameBuffer {
public:
	OpenGLFrameBuffer(const FrameBufferSpecification& specs);
	~OpenGLFrameBuffer() override;
	void bind() override;
	void unbind() override;
	void resize(const WindowSize& size) override;
	int get_pixel_value(uint32_t attachment_index, int x, int y) override;
	void clear_attachment(uint32_t attachment_index, int value) override;

	const FrameBufferSpecification& get_specification() const override { return specifications_; }

	uint32_t get_color_attachment_renderer_id(uint32_t index = 0) const override {
		EC_CORE_ASSERT(index < color_attachment_ids_.size(),
		               "Trying to access color attachment id larger than the number of color attachments!");
		return color_attachment_ids_[index];
	}

	void invalidate();

private:
	void reset();

	uint32_t renderer_id_ = 0;
	FrameBufferSpecification specifications_;

	std::vector<FramebufferTextureSpecification> color_attachment_specs_;
	FramebufferTextureSpecification depth_attachment_spec_ = FramebufferTextureFormat::none;

	std::vector<uint32_t> color_attachment_ids_;
	uint32_t depth_attachment_id_;
};

}  // namespace eclipse