#pragma once

#include <vector>

#include "eclipse/core/core.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

enum class FramebufferTextureFormat { none, rgba8, red_integer, depth24stencil8, depth = depth24stencil8 };

namespace utils {

static inline bool is_depth_format(FramebufferTextureFormat format) {
	using enum FramebufferTextureFormat;
	return format == depth24stencil8;
}
}  // namespace utils

struct FramebufferTextureSpecification {
	FramebufferTextureSpecification() = default;

	FramebufferTextureSpecification(const FramebufferTextureFormat& format) : texture_format(format) {}

	FramebufferTextureFormat texture_format = FramebufferTextureFormat::none;
};

struct FramebufferAttachmentSpecification {
	FramebufferAttachmentSpecification() = default;

	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachment)
	    : attachments(attachment) {}

	std::vector<FramebufferTextureSpecification> attachments;
};

struct FrameBufferSpecification {
	au::Quantity<units::Pixels, uint32_t> width;
	au::Quantity<units::Pixels, uint32_t> height;
	FramebufferAttachmentSpecification attachments;
	int32_t samples        = 1;
	bool swap_chain_target = false;
};

class FrameBuffer {
public:
	static ref<FrameBuffer> create(const FrameBufferSpecification& specs);
	virtual ~FrameBuffer()                                                      = default;
	virtual void bind()                                                         = 0;
	virtual void unbind()                                                       = 0;
	virtual void resize(const WindowSize& size)                                 = 0;
	virtual int get_pixel_value(uint32_t attachment_index, int x, int y)        = 0;
	virtual void clear_attachment(uint32_t attachment_index, int value)         = 0;
	virtual const FrameBufferSpecification& get_specification() const           = 0;
	virtual uint32_t get_color_attachment_renderer_id(uint32_t index = 0) const = 0;

private:
};

}  // namespace eclipse