#include "ecpch.h"
#include "opengl_frame_buffer.h"

#include <glad/glad.h>

namespace eclipse {
OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specs) : specifications_(specs) { invalidate(); }

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
	EC_PROFILE_FUNCTION();
	glDeleteFramebuffers(1, &renderer_id_);
}

void OpenGLFrameBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_); }

void OpenGLFrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFrameBuffer::invalidate() {
	EC_PROFILE_FUNCTION();

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
	//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, specifications_.width, specifications_.height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, specifications_.width, specifications_.height, 0, GL_DEPTH_STENCIL,
	             GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment_, 0);

	EC_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
}  // namespace eclipse