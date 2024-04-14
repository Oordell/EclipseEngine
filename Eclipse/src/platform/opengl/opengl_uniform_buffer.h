#pragma once

#include "eclipse/renderer/uniform_buffer.h"

namespace eclipse {

class OpenGLUniformBuffer : public UniformBuffer {
public:
	OpenGLUniformBuffer(const UniformBufferRecipe& args);
	virtual ~OpenGLUniformBuffer();
	virtual void set_data(const void* data, const UniformBufferDataDescription& details) override;

private:
	uint32_t renderer_id_ = 0;
};

}  // namespace eclipse