#pragma once

#include "eclipse/renderer/texture.h"

namespace eclipse {

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(const std::string& path);
	virtual ~OpenGLTexture2D();

	uint32_t get_width() const override { return width_; }

	uint32_t get_height() const override { return height_; }

	void bind(uint32_t slot = 0) const override;

private:
	std::string path_;
	uint32_t width_;
	uint32_t height_;
	uint32_t renderer_id_;
};

}  // namespace eclipse