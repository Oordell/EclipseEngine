#include "ecpch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

OpenGLShader::OpenGLShader(const std::string& vertex_src, const std::string& fragment_src) {
	// Create an empty vertex shader handle
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Send the vertex shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	const GLchar* source = (const GLchar*) vertex_src.c_str();
	glShaderSource(vertexShader, 1, &source, 0);

	// Compile the vertex shader
	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(vertexShader);

		EC_CORE_FATAL("{0}", infoLog.data());
		EC_CORE_ASSERT(false, "Vertex shader compilation failure!");
		return;
	}

	// Create an empty fragment shader handle
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Send the fragment shader source code to GL
	// Note that std::string's .c_str is NULL character terminated.
	source = (const GLchar*) fragment_src.c_str();
	glShaderSource(fragmentShader, 1, &source, 0);

	// Compile the fragment shader
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

		// We don't need the shader anymore.
		glDeleteShader(fragmentShader);
		// Either of them. Don't leak shaders.
		glDeleteShader(vertexShader);

		EC_CORE_FATAL("{0}", infoLog.data());
		EC_CORE_ASSERT(false, "Fragment shader compilation failure!");
		return;
	}

	// Vertex and fragment shaders are successfully compiled.
	// Now time to link them together into a program.
	// Get a program object.
	renderer_id_ = glCreateProgram();

	// Attach our shaders to our program
	glAttachShader(renderer_id_, vertexShader);
	glAttachShader(renderer_id_, fragmentShader);

	// Link our renderer_id_
	glLinkProgram(renderer_id_);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(renderer_id_, GL_LINK_STATUS, (int*) &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(renderer_id_, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(renderer_id_, maxLength, &maxLength, &infoLog[0]);

		// We don't need the renderer_id_ anymore.
		glDeleteProgram(renderer_id_);
		// Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		EC_CORE_FATAL("{0}", infoLog.data());
		EC_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	// Always detach shaders after a successful link.
	glDetachShader(renderer_id_, vertexShader);
	glDetachShader(renderer_id_, fragmentShader);
}

OpenGLShader::~OpenGLShader() { glDeleteProgram(renderer_id_); }

void OpenGLShader::bind() const { glUseProgram(renderer_id_); }

void OpenGLShader::unbind() const { glUseProgram(0); }

void OpenGLShader::upload_uniform_int(const std::string& name, int value) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1i(location, value);
}

void OpenGLShader::upload_uniform_float(const std::string& name, float value) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1f(location, value);
}

void OpenGLShader::upload_uniform_float2(const std::string& name, const glm::vec2& values) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform2f(location, values.x, values.y);
}

void OpenGLShader::upload_uniform_float3(const std::string& name, const glm::vec3& values) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform3f(location, values.x, values.y, values.z);
}

void OpenGLShader::upload_uniform_float4(const std::string& name, const glm::vec4& values) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform4f(location, values.x, values.y, values.z, values.w);
}

void OpenGLShader::upload_uniform_mat3(const std::string& name, const glm::mat4& matrix) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::upload_uniform_mat4(const std::string& name, const glm::mat4& matrix) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}  // namespace eclipse