#include "ecpch.h"
#include "opengl_context.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace eclipse {

	OpenGLContext::OpenGLContext(GLFWwindow* window_handle) : window_handle_(window_handle) {
		EC_CORE_ASSERT(window_handle_, "Window handle is null!");
	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(window_handle_);
		int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		EC_CORE_ASSERT(status, "Failed to initialize Glad!")
	}

	void OpenGLContext::swap_buffers() {
		glfwSwapBuffers(window_handle_);
	}

}