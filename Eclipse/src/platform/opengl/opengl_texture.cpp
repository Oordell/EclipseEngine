#include "ecpch.h"
#include "opengl_texture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace eclipse {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : path_(path) {
	int width    = 0;
	int height   = 0;
	int channels = 0;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	EC_CORE_ASSERT(data, "Failed to load image!");

	width_  = static_cast<uint32_t>(width);
	height_ = static_cast<uint32_t>(height);

	GLenum internal_format = 0;
	GLenum data_format     = 0;
	if (channels == 4) {
		internal_format = GL_RGBA8;
		data_format     = GL_RGBA;
	} else if (channels == 3) {
		internal_format = GL_RGB8;
		data_format     = GL_RGB;
	}
	EC_CORE_ASSERT(internal_format & data_format, "Texture format not suported!. Channels: {0}", channels);

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, internal_format, width_, height_);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(renderer_id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, data_format, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &renderer_id_); }

void OpenGLTexture2D::bind(uint32_t slot) const { glBindTextureUnit(slot, renderer_id_); }

}  // namespace eclipse