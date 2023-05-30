#pragma once

#include "eclipse/renderer/graphics_context.h"

struct GLFWwindow;

namespace eclipse {

class ECLIPSE_API OpenGLContext : public GraphicsContext {
public:
	OpenGLContext(GLFWwindow* window_handle);

	void init() override;
	void swap_buffers() override;

private:
	GLFWwindow* window_handle_;
};

}  // namespace eclipse