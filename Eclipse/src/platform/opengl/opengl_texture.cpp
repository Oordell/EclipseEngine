#include "ecpch.h"
#include "opengl_texture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace eclipse {

OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : path_(path) {
	int width     = 0;
	int height    = 0;
	int channels  = 0;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	EC_CORE_ASSERT(data, "Failed to load image!");

	width_  = static_cast<uint32_t>(width);
	height_ = static_cast<uint32_t>(height);

	glCreateTextures(GL_TEXTURE_2D, 1, &renderer_id_);
	glTextureStorage2D(renderer_id_, 1, GL_RGB8, width_, height_);

	glTextureParameteri(renderer_id_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(renderer_id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureSubImage2D(renderer_id_, 0, 0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D() { glDeleteTextures(1, &renderer_id_); }

void OpenGLTexture2D::bind(uint32_t slot) const { glBindTextureUnit(slot, renderer_id_); }

}  // namespace eclipse