#include "ecpch.h"
#include "opengl_frame_buffer.h"

#include <glad/glad.h>

namespace eclipse {

namespace utils {
static bool is_multisampling(int num_of_samples) { return num_of_samples > 1; }

static GLenum texture_target(bool multisampling) { return multisampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; }

static void create_textures(bool multisampling, uint32_t* out_id, uint32_t count) {
	glCreateTextures(texture_target(multisampling), count, out_id);
}

static void bind_texture(bool multisampling, uint32_t id) { glBindTexture(texture_target(multisampling), id); }

static void apply_mono_sampling_filtering() {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

static void attach_color_texture(uint32_t id, int samples, GLenum internal_format, GLenum format, uint32_t width,
                                 uint32_t height, int index) {
	bool multisampling = is_multisampling(samples);
	if (multisampling) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_FALSE);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
		apply_mono_sampling_filtering();
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture_target(multisampling), id, 0);
}

static void attach_depth_texture(uint32_t id, int samples, GLenum format, GLenum attachment_type, uint32_t width,
                                 uint32_t height) {
	bool multisampling = is_multisampling(samples);
	if (multisampling) {
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	} else {
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
		apply_mono_sampling_filtering();
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, texture_target(multisampling), id, 0);
}

static GLenum eclipse_frame_buffer_texture_format_to_gl_format(FramebufferTextureFormat format) {
	using enum FramebufferTextureFormat;
	switch (format) {
		case rgba8: {
			return GL_RGBA8;
		}
		case red_integer: {
			return GL_RED_INTEGER;
		}
		case none:
		case depth24stencil8: {
			EC_CORE_ASSERT(false, "Unsupported FrameBufferTextureFormat supplied!: {0}", static_cast<int>(format));
		}
	}
	return 0;
}
}  // namespace utils

OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpecification& specs) : specifications_(specs) {
	for (auto format : specifications_.attachments.attachments) {
		if (!utils::is_depth_format(format.texture_format)) {
			color_attachment_specs_.emplace_back(format);
		} else {
			depth_attachment_spec_ = format;
		}
	}
	invalidate();
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() {
	EC_PROFILE_FUNCTION();
	reset();
}

void OpenGLFrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
	glViewport(0, 0, specifications_.width.in(units::pixels), specifications_.height.in(units::pixels));
}

void OpenGLFrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFrameBuffer::resize(const WindowSize& size) {
	static const auto MAX_FRAME_BUFFER_SIZE = units::pixels(8192);
	if (size.width == units::pixels(0) || size.height == units::pixels(0) || size.width > MAX_FRAME_BUFFER_SIZE ||
	    size.height > MAX_FRAME_BUFFER_SIZE) {
		EC_CORE_ERROR("Invalid frame buffer resize attempt. Tried resizing to: ({0}, {1})", size.width, size.height);
		return;
	}

	specifications_.width  = size.width;
	specifications_.height = size.height;
	invalidate();
}

int OpenGLFrameBuffer::get_pixel_value(uint32_t attachment_index, int x, int y) {
	EC_CORE_ASSERT(attachment_index < color_attachment_ids_.size(),
	               "Trying to read pixel value from a attachment index out of bounds! Number of attachments: {0}, "
	               "Attachment index: {1}",
	               color_attachment_ids_.size(), attachment_index);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachment_index);
	int pixel_data = -1;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);
	return pixel_data;
}

void OpenGLFrameBuffer::clear_attachment(uint32_t attachment_index, int value) {
	EC_CORE_ASSERT(attachment_index < color_attachment_ids_.size(),
	               "Trying to read pixel value from a attachment index out of bounds! Number of attachments: {0}, "
	               "Attachment index: {1}",
	               color_attachment_ids_.size(), attachment_index);

	auto& specs = color_attachment_specs_[attachment_index];
	glClearTexImage(color_attachment_ids_[attachment_index], 0,
	                utils::eclipse_frame_buffer_texture_format_to_gl_format(specs.texture_format), GL_INT, &value);
}

void OpenGLFrameBuffer::invalidate() {
	EC_PROFILE_FUNCTION();

	if (renderer_id_ != 0) {
		reset();
	}

	glCreateFramebuffers(1, &renderer_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);

	bool multisampling = utils::is_multisampling(specifications_.samples);

	// Attachments
	if (!color_attachment_specs_.empty()) {
		color_attachment_ids_.resize(color_attachment_specs_.size());
		utils::create_textures(multisampling, color_attachment_ids_.data(),
		                       static_cast<uint32_t>(color_attachment_ids_.size()));

		for (size_t i = 0; i < color_attachment_ids_.size(); i++) {
			utils::bind_texture(multisampling, color_attachment_ids_[i]);

			switch (color_attachment_specs_[i].texture_format) {
				case FramebufferTextureFormat::rgba8: {
					utils::attach_color_texture(color_attachment_ids_[i], specifications_.samples, GL_RGBA8, GL_RGBA,
					                            specifications_.width.in(units::pixels), specifications_.height.in(units::pixels),
					                            static_cast<int>(i));
					break;
				}
				case FramebufferTextureFormat::red_integer: {
					utils::attach_color_texture(color_attachment_ids_[i], specifications_.samples, GL_R32I, GL_RED_INTEGER,
					                            specifications_.width.in(units::pixels), specifications_.height.in(units::pixels),
					                            static_cast<int>(i));
					break;
				}
				default: {
					break;
				}
			}
		}
	}

	if (depth_attachment_spec_.texture_format != FramebufferTextureFormat::none) {
		utils::create_textures(multisampling, &depth_attachment_id_, 1);
		utils::bind_texture(multisampling, depth_attachment_id_);
		switch (depth_attachment_spec_.texture_format) {
			case FramebufferTextureFormat::depth24stencil8: {
				utils::attach_depth_texture(depth_attachment_id_, specifications_.samples, GL_DEPTH24_STENCIL8,
				                            GL_DEPTH_STENCIL_ATTACHMENT, specifications_.width.in(units::pixels),
				                            specifications_.height.in(units::pixels));
				break;
			}
		}
	}

	if (color_attachment_ids_.size() > 1) {
		EC_CORE_ASSERT(color_attachment_ids_.size() <= 4, "Eclipse don't support more than 4 color attachments!");
		GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(static_cast<GLsizei>(color_attachment_ids_.size()), buffers);
	} else if (color_attachment_ids_.empty()) {
		glDrawBuffer(GL_NONE);
	}

	EC_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFrameBuffer::reset() {
	glDeleteFramebuffers(1, &renderer_id_);
	glDeleteTextures(static_cast<GLsizei>(color_attachment_ids_.size()), color_attachment_ids_.data());
	glDeleteTextures(1, &depth_attachment_id_);
	color_attachment_ids_.clear();
	depth_attachment_id_ = 0;
}
}  // namespace eclipse