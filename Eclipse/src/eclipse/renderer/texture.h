#pragma once

#include "eclipse/core.h"
#include <string>

namespace eclipse {

class ECLIPSE_API Texture {
public:
	virtual ~Texture()                  = default;
	virtual uint32_t get_width() const  = 0;
	virtual uint32_t get_height() const = 0;
	virtual void bind(uint32_t slot) const = 0;
};

class ECLIPSE_API Texture2D : public Texture {
public:
	static ref<Texture2D> create(const std::string& path);

};

}  // namespace eclipse