#include "ecpch.h"
#include "opengl_shader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

static GLenum shader_type_from_string(const std::string& type) {
	if (type == "vertex") {
		return GL_VERTEX_SHADER;
	} else if (type == "fragment" || type == "pixel") {
		return GL_FRAGMENT_SHADER;
	}
	EC_CORE_ASSERT(false, "Unknown shader type! '{0}'", type);
	return 0;
}

OpenGLShader::OpenGLShader(const FilePath& filepath) {
	auto source      = read_file(filepath);
	auto shader_srcs = pre_process(source);
	compile(shader_srcs);
}

OpenGLShader::OpenGLShader(const std::string& vertex_src, const std::string& fragment_src) {
	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER]   = vertex_src;
	sources[GL_FRAGMENT_SHADER] = fragment_src;
	compile(sources);
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

std::string OpenGLShader::read_file(const FilePath& filepath) {
	std::string result;
	std::ifstream in(filepath.value(), std::ios::in, std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	} else {
		EC_CORE_ERROR("Couldn't open file '{0}'", filepath.value());
	}
	return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::pre_process(const std::string& vertes_src) {
	std::unordered_map<GLenum, std::string> shader_srcs;

	const char* type_token   = "#type";
	size_t type_token_length = strlen(type_token);

	size_t pos = vertes_src.find(type_token, 0);
	while (pos != std::string::npos) {
		size_t eol = vertes_src.find_first_of("\r\n", pos);
		EC_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin     = pos + type_token_length + 1;
		std::string type = vertes_src.substr(begin, eol - begin);
		EC_CORE_ASSERT(shader_type_from_string(type), "Invalid shader type specified");

		size_t next_line_pos                       = vertes_src.find_first_not_of("\r\n", eol);
		pos                                        = vertes_src.find(type_token, next_line_pos);
		shader_srcs[shader_type_from_string(type)] = vertes_src.substr(
		    next_line_pos, pos - (next_line_pos == std::string::npos ? vertes_src.size() - 1 : next_line_pos));
	}
	return shader_srcs;
}

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shader_srcs) {
	GLuint program = glCreateProgram();
	std::vector<GLenum> glShaderIDs(shader_srcs.size());
	for (auto& kv : shader_srcs) {
		GLenum type               = kv.first;
		const std::string& source = kv.second;

		GLuint shader = glCreateShader(type);

		const GLchar* source_cstr = source.c_str();
		glShaderSource(shader, 1, &source_cstr, 0);

		glCompileShader(shader);

		GLint is_compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE) {
			GLint max_length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(shader, max_length, &max_length, &info_log[0]);

			glDeleteShader(shader);

			EC_CORE_ERROR("{0}", info_log.data());
			EC_CORE_ASSERT(false, "Shader compilation failure!");
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs.push_back(shader);
	}

	renderer_id_ = program;

	// Link our program
	glLinkProgram(program);

	// Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint is_linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*) &is_linked);
	if (is_linked == GL_FALSE) {
		GLint max_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);
		// The max_length includes the NULL character
		std::vector<GLchar> info_log(max_length);
		glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

		// We don't need the program anymore.
		glDeleteProgram(program);

		for (auto id : glShaderIDs) {
			glDeleteShader(id);
		}

		EC_CORE_ERROR("{0}", info_log.data());
		EC_CORE_ASSERT(false, "Shader link failure!");
		return;
	}

	for (auto id : glShaderIDs) {
		glDetachShader(program, id);
	}
}

}  // namespace eclipse