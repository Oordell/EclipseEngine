#pragma once

#include "eclipse/core.h"

namespace eclipse {

class ECLIPSE_API IndexBuffer {
public:
	virtual ~IndexBuffer() = default;

	virtual void bind() const          = 0;
	virtual void unbind() const        = 0;
	virtual uint32_t get_count() const = 0;

	static IndexBuffer* create(uint32_t* indices, uint32_t count);
};

}  // namespace eclipse