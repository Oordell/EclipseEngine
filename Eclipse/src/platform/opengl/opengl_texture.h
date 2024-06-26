#pragma once

#include "eclipse/renderer/texture.h"

#include <glad/glad.h>

namespace eclipse {

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(const WindowSize& size);
	OpenGLTexture2D(const std::string& path);
	virtual ~OpenGLTexture2D();

	au::Quantity<units::Pixels, uint32_t> get_width() const override { return width_; }

	au::Quantity<units::Pixels, uint32_t> get_height() const override { return height_; }

	uint32_t get_renderer_id() const override { return renderer_id_; }

	std::optional<std::string> get_path() const override {
		if (path_.empty()) {
			return std::nullopt;
		}
		return path_;
	}

	void set_data(void* data, uint32_t size) override;

	void bind(uint32_t slot = 0) const override;

private:
	std::string path_ {};
	au::Quantity<units::Pixels, uint32_t> width_;
	au::Quantity<units::Pixels, uint32_t> height_;
	uint32_t renderer_id_;
	GLenum internal_format_;
	GLenum data_format_;
};

}  // namespace eclipse