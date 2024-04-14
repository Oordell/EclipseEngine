#pragma once

#include "eclipse/core/core.h"

namespace eclipse {

struct UniformBufferRecipe {
	uint32_t size    = 0;
	uint32_t binding = 0;
};

struct UniformBufferDataDescription {
	uint32_t size   = 0;
	uint32_t offset = 0;
};

class UniformBuffer {
public:
	virtual ~UniformBuffer()                                                             = default;
	virtual void set_data(const void* data, const UniformBufferDataDescription& details) = 0;
	static ref<UniformBuffer> create(const UniformBufferRecipe& args);
};

}  // namespace eclipse