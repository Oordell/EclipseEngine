#pragma once

#include "eclipse/core/core.h"
#include <string>
#include "eclipse/common_types/window_size.h"

namespace eclipse {

class ECLIPSE_API Texture {
public:
	virtual ~Texture()                               = default;
	virtual uint32_t get_width() const               = 0;
	virtual uint32_t get_height() const              = 0;
	virtual uint32_t get_renderer_id() const         = 0;
	virtual void set_data(void* data, uint32_t size) = 0;
	virtual void bind(uint32_t slot = 0) const       = 0;
};

class ECLIPSE_API Texture2D : public Texture {
public:
	static ref<Texture2D> create(const WindowSize& size);
	static ref<Texture2D> create(const std::string& path);

private:
	template <typename T>
	static ref<Texture2D> create_impl(const T& arg);
};

}  // namespace eclipse