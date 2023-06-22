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
	EC_CORE_ASSERT(status, "Failed to initialize Glad!");
	EC_CORE_INFO("OpenGL Info:");
	EC_CORE_INFO(" -> Vendor:   {0}", std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR))));
	EC_CORE_INFO(" -> Renderer: {0}", std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));
	EC_CORE_INFO(" -> Version:  {0}", std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));

#ifdef HZ_ENABLE_ASSERTS
	int version_major;
	int version_minor;
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);

	EC_CORE_ASSERT(version_major > 4 || (version_major == 4 && version_minor >= 5),
	               "Eclipse requires at least OpenGL version 4.5!");
#endif
}

void OpenGLContext::swap_buffers() { glfwSwapBuffers(window_handle_); }

}  // namespace eclipse