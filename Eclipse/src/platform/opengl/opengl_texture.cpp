#include "ecpch.h"
#include "opengl_texture.h"

#include "stb_image.h"

namespace eclipse {
OpenGLTexture2D::OpenGLTexture2D(const WindowSize& size) : width_(size.width), height_(size.height) {
	EC_PROFILE_FUNCTION();

	internal_format_ = GL_RGBA8;
	data_format_     = GL_RGBA;

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, internal_format_, width_.in(units::pixels), height_.in(units::pixels));

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : path_(path) {
	EC_PROFILE_FUNCTION();

	int width    = 0;
	int height   = 0;
	int channels = 0;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = nullptr;
	{
		EC_PROFILE_SCOPE("OpenGLTexture2D::OpenGLTexture2D(const std::string&)::stbi_load");
		data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	}
	EC_CORE_ASSERT(data, "Failed to load image!");

	width_  = units::pixels(static_cast<uint32_t>(width));
	height_ = units::pixels(static_cast<uint32_t>(height));

	GLenum internal_format = 0;
	GLenum data_format     = 0;
	if (channels == 4) {
		internal_format = GL_RGBA8;
		data_format     = GL_RGBA;
	} else if (channels == 3) {
		internal_format = GL_RGB8;
		data_format     = GL_RGB;
	}

	internal_format_ = internal_format;
	data_format_     = data_format;

	EC_CORE_ASSERT(internal_format & data_format, "Texture format not suported!. Channels: {0}", channels);

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, internal_format, width_.in(units::pixels), height_.in(units::pixels));

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_.in(units::pixels), height_.in(units::pixels), data_format,
	                    GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() {
	EC_PROFILE_FUNCTION();

	glDeleteTextures(1, &renderer_id_);
}

void OpenGLTexture2D::set_data(void* data, uint32_t size) {
	EC_PROFILE_FUNCTION();

	uint32_t bytes_pr_pixel = data_format_ == GL_RGBA ? 4 : 3;
	EC_CORE_ASSERT(size == (width_ * height_).in(au::squared(units::pixels)) * bytes_pr_pixel,
	               "Data must be entire texture");
	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_.in(units::pixels), height_.in(units::pixels), data_format_,
	                    GL_UNSIGNED_BYTE, data);
}

void OpenGLTexture2D::bind(uint32_t slot) const {
	EC_PROFILE_FUNCTION();

	glBindTextureUnit(slot, renderer_id_);
}

}  // namespace eclipse