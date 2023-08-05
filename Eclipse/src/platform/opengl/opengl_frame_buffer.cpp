#include "ecpch.h"
#include "opengl_frame_buffer.h"

#include <glad/glad.h>

namespace eclipse {
OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specs) : specifications_(specs) { invalidate(); }

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
	EC_PROFILE_FUNCTION();
	reset();
}

void OpenGLFrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
	glViewport(0, 0, specifications_.width, specifications_.height);
}

void OpenGLFrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFrameBuffer::resize(const WindowSize& size) {
	static const uint32_t MAX_FRAME_BUFFER_SIZE = 8192;
	if (size.width == 0 || size.height == 0 || size.width > MAX_FRAME_BUFFER_SIZE || size.height > MAX_FRAME_BUFFER_SIZE) {
		EC_CORE_ERROR("Invalid frame buffer resize attempt. Tried resizing to: ({0}, {1})", size.width, size.height);
		return;
	}

	specifications_.width  = size.width;
	specifications_.height = size.height;
	invalidate();
}

void OpenGLFrameBuffer::invalidate() {
	EC_PROFILE_FUNCTION();

	if (renderer_id_ != 0) {
		reset();
	}

	glCreateFramebuffers(1, &renderer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

	glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_);
	glBindTexture(GL_TEXTURE_2D, color_attachment_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specifications_.width, specifications_.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
	             nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_);
	glBindTexture(GL_TEXTURE_2D, depth_attachment_);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, specifications_.width, specifications_.height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

	EC_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::reset() {
	glDeleteFramebuffers(1, &renderer_id_);
	glDeleteTextures(1, &color_attachment_);
	glDeleteTextures(1, &depth_attachment_);
}
}  // namespace eclipse