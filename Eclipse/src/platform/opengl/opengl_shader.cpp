#include "ecpch.h"
#include "opengl_shader.h"
#include "eclipse/core/timer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace eclipse {

namespace utils {

static GLenum shader_type_from_string(const std::string& type) {
	if (type == "vertex") {
		return GL_VERTEX_SHADER;
	} else if (type == "fragment" || type == "pixel") {
		return GL_FRAGMENT_SHADER;
	}
	EC_CORE_ASSERT(false, "Unknown shader type! '{0}'", type);
	return 0;
}

static shaderc_shader_kind gl_shader_stage_to_shaderc(GLenum stage) {
	switch (stage) {
		case GL_VERTEX_SHADER: {
			return shaderc_glsl_vertex_shader;
		}
		case GL_FRAGMENT_SHADER: {
			return shaderc_glsl_fragment_shader;
		}
	}
	EC_CORE_ASSERT(false, "The shader of stage type {0} is not supported", stage);
	return static_cast<shaderc_shader_kind>(0);
}

static std::string gl_shader_stage_to_string(GLenum stage) {
	switch (stage) {
		case GL_VERTEX_SHADER: {
			return "GL_VERTEX_SHADER";
		}
		case GL_FRAGMENT_SHADER: {
			return "GL_FRAGMENT_SHADER";
		}
	}
	EC_CORE_ASSERT(false, "The shader of stage type {0} is not supported", stage);
	return nullptr;
}

static FilePath get_cache_directory() {
	// TODO: make sure the assets directory is valid
	static const FilePath path {"assets/cache/shader/opengl"};
	return path;
}

static void create_cache_directory_if_needed() {
	auto cache_directory = get_cache_directory();
	if (!std::filesystem::exists(cache_directory.value())) {
		std::filesystem::create_directories(cache_directory.value());
	}
}

static std::string gl_shader_stage_cached_opengl_file_extension(GLenum stage) {
	switch (stage) {
		case GL_VERTEX_SHADER: {
			return ".cached_opengl.vert";
		}
		case GL_FRAGMENT_SHADER: {
			return ".cached_opengl.frag";
		}
	}
	EC_CORE_ASSERT(false, "The shader of stage type {0} is not supported", stage);
	return "";
}

static std::string gl_shader_stage_cached_vulkan_file_extension(GLenum stage) {
	switch (stage) {
		case GL_VERTEX_SHADER: {
			return ".cached_vulkan.vert";
		}
		case GL_FRAGMENT_SHADER: {
			return ".cached_vulkan.frag";
		}
	}
	EC_CORE_ASSERT(false, "The shader of stage type {0} is not supported", stage);
	return "";
}

}  // namespace utils

OpenGLShader::OpenGLShader(const FilePath& filepath) : file_path_(filepath) {
	EC_PROFILE_FUNCTION();

	utils::create_cache_directory_if_needed();

	std::filesystem::path path = filepath.value();
	name_                      = path.stem().string();  // Returns the file's name stripped of the extension.

	auto source      = read_file(filepath);
	auto shader_srcs = pre_process(source);

	init(shader_srcs);
}

OpenGLShader::OpenGLShader(const ShaderInfo& info) : name_(info.name) {
	EC_PROFILE_FUNCTION();

	std::unordered_map<GLenum, std::string> sources;
	sources[GL_VERTEX_SHADER]   = info.vertex_src;
	sources[GL_FRAGMENT_SHADER] = info.fragment_src;

	init(sources);
}

void OpenGLShader::init(const std::unordered_map<GLenum, std::string>& sources) {
	Timer timer;
	compile_or_get_vulkan_binaries(sources);
	compile_or_get_opengl_binaries();
	create_program();
	EC_CORE_DEBUG("Shader creation took {} ms", timer.elapsed_milliseconds().in<au::Milli<au::Seconds>>());
}

OpenGLShader::~OpenGLShader() { glDeleteProgram(renderer_id_); }

void OpenGLShader::bind() const {
	EC_PROFILE_FUNCTION();
	glUseProgram(renderer_id_);
}

void OpenGLShader::unbind() const {
	EC_PROFILE_FUNCTION();
	glUseProgram(0);
}

void OpenGLShader::set_int(const std::string& name, int value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_int(name, value);
}

void OpenGLShader::set_int_array(const std::string& name, int* values, uint32_t count) {
	EC_PROFILE_FUNCTION();
	upload_uniform_int_array(name, values, count);
}

void OpenGLShader::set_float(const std::string& name, float value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_float(name, value);
}

void OpenGLShader::set_float2(const std::string& name, const glm::vec2& value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_float2(name, value);
}

void OpenGLShader::set_float3(const std::string& name, const glm::vec3& value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_float3(name, value);
}

void OpenGLShader::set_float4(const std::string& name, const glm::vec4& value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_float4(name, value);
}

void OpenGLShader::set_mat3(const std::string& name, const glm::mat3& value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_mat3(name, value);
}

void OpenGLShader::set_mat4(const std::string& name, const glm::mat4& value) {
	EC_PROFILE_FUNCTION();
	upload_uniform_mat4(name, value);
}

void OpenGLShader::upload_uniform_int(const std::string& name, int value) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1i(location, value);
}

void OpenGLShader::upload_uniform_int_array(const std::string& name, int* values, uint32_t count) {
	auto location = glGetUniformLocation(renderer_id_, name.c_str());
	glUniform1iv(location, count, values);
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
	EC_PROFILE_FUNCTION();

	std::string result;
	std::ifstream in(filepath.value(), std::ios::in | std::ios::binary);
	if (in) {
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1) {
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(&result[0], size);
			in.close();
		} else {
			EC_CORE_ERROR("Could not read from file '{0}'", filepath.value());
		}
	} else {
		EC_CORE_ERROR("Couldn't open file '{0}'", filepath.value());
	}
	return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::pre_process(const std::string& vertes_src) {
	EC_PROFILE_FUNCTION();

	std::unordered_map<GLenum, std::string> shader_srcs;

	const char* type_token   = "#type";
	size_t type_token_length = strlen(type_token);

	size_t pos = vertes_src.find(type_token, 0);
	while (pos != std::string::npos) {
		size_t eol = vertes_src.find_first_of("\r\n", pos);
		EC_CORE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin     = pos + type_token_length + 1;
		std::string type = vertes_src.substr(begin, eol - begin);
		EC_CORE_ASSERT(utils::shader_type_from_string(type), "Invalid shader type specified");

		size_t next_line_pos = vertes_src.find_first_not_of("\r\n", eol);
		EC_CORE_ASSERT(next_line_pos != std::string::npos, "Syntax error");
		pos                                               = vertes_src.find(type_token, next_line_pos);
		shader_srcs[utils::shader_type_from_string(type)] = (pos == std::string::npos)
		                                                        ? vertes_src.substr(next_line_pos)
		                                                        : vertes_src.substr(next_line_pos, pos - next_line_pos);
	}
	return shader_srcs;
}

void OpenGLShader::compile_or_get_vulkan_binaries(const std::unordered_map<GLenum, std::string>& shader_srcs) {
	EC_PROFILE_FUNCTION();

	GLuint program = glCreateProgram();

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
	constexpr bool optimize = true;
	if (optimize) {
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
	}

	std::filesystem::path cache_directory = utils::get_cache_directory().value();

	auto& shader_data = vulkan_spirv_;
	shader_data.clear();
	for (auto&& [stage, source] : shader_srcs) {
		std::filesystem::path shader_file_path = file_path_.value();
		std::filesystem::path cached_path      = cache_directory / (shader_file_path.filename().string() +
                                                         utils::gl_shader_stage_cached_vulkan_file_extension(stage));

		std::ifstream in(cached_path, std::ios::in | std::ios::binary);
		if (in.is_open()) {
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = shader_data[stage];
			data.resize(size / sizeof(uint32_t));
			in.read((char*) data.data(), size);
		} else {
			shaderc::SpvCompilationResult module =
			    compiler.CompileGlslToSpv(source, utils::gl_shader_stage_to_shaderc(stage), file_path_.value().c_str(), options);
			if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
				EC_CORE_ERROR(module.GetErrorMessage());
				EC_CORE_ASSERT(false, "Failed to compile shader!");
			}

			shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

			std::ofstream out(cached_path, std::ios::out | std::ios::binary);
			if (out.is_open()) {
				auto& data = shader_data[stage];
				out.write((char*) data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}

	for (auto&& [stage, data] : shader_data) {
		reflect(stage, data);
	}
}

void OpenGLShader::compile_or_get_opengl_binaries() {
	EC_PROFILE_FUNCTION();

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
	constexpr bool optimize = false;
	if (optimize) {
		options.SetOptimizationLevel(shaderc_optimization_level_performance);
	}

	std::filesystem::path cache_directory = utils::get_cache_directory().value();

	auto& shader_data = opengl_spirv_;
	shader_data.clear();
	opengl_source_code_.clear();
	for (auto&& [stage, spirv] : vulkan_spirv_) {
		std::filesystem::path shader_file_path = file_path_.value();
		std::filesystem::path cached_path      = cache_directory / (shader_file_path.filename().string() +
                                                         utils::gl_shader_stage_cached_opengl_file_extension(stage));

		std::ifstream in(cached_path, std::ios::in | std::ios::binary);
		if (in.is_open()) {
			in.seekg(0, std::ios::end);
			auto size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = shader_data[stage];
			data.resize(size / sizeof(uint32_t));
			in.read((char*) data.data(), size);
		} else {
			spirv_cross::CompilerGLSL glslCompiler(spirv);
			opengl_source_code_[stage] = glslCompiler.compile();
			auto& source               = opengl_source_code_[stage];

			shaderc::SpvCompilationResult module =
			    compiler.CompileGlslToSpv(source, utils::gl_shader_stage_to_shaderc(stage), file_path_.value().c_str());
			if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
				EC_CORE_ERROR(module.GetErrorMessage());
				EC_CORE_ASSERT(false, "Failed to compile shader code!");
			}

			shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

			std::ofstream out(cached_path, std::ios::out | std::ios::binary);
			if (out.is_open()) {
				auto& data = shader_data[stage];
				out.write((char*) data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}
		}
	}
}

void OpenGLShader::create_program() {
	EC_PROFILE_FUNCTION();

	GLuint program = glCreateProgram();

	std::vector<GLuint> shader_ids;
	for (auto&& [stage, spirv] : opengl_spirv_) {
		GLuint shader_id = shader_ids.emplace_back(glCreateShader(stage));
		glShaderBinary(1, &shader_id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(),
		               static_cast<GLsizei>(spirv.size() * sizeof(uint32_t)));
		glSpecializeShader(shader_id, "main", 0, nullptr, nullptr);
		glAttachShader(program, shader_id);
	}

	glLinkProgram(program);

	GLint is_linked;
	glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
	if (is_linked == GL_FALSE) {
		GLint max_length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

		std::vector<GLchar> info_log(max_length);
		glGetProgramInfoLog(program, max_length, &max_length, info_log.data());
		EC_CORE_ERROR("Shader linking failed ({0}):\n{1}", file_path_.value(), info_log.data());

		glDeleteProgram(program);

		for (auto id : shader_ids) {
			glDeleteShader(id);
		}
	}

	for (auto id : shader_ids) {
		glDetachShader(program, id);
		glDeleteShader(id);
	}

	renderer_id_ = program;
}

void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shader_data) {
	spirv_cross::Compiler compiler(shader_data);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	EC_CORE_TRACE("OpenGLShader::reflect - {0} {1}", utils::gl_shader_stage_to_string(stage), file_path_.value());
	EC_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
	EC_CORE_TRACE("    {0} resources", resources.sampled_images.size());

	EC_CORE_TRACE("Uniform buffers:");
	for (const auto& resource : resources.uniform_buffers) {
		const auto& buffer_type = compiler.get_type(resource.base_type_id);
		uint32_t buffer_size    = static_cast<uint32_t>(compiler.get_declared_struct_size(buffer_type));
		uint32_t binding        = compiler.get_decoration(resource.id, spv::DecorationBinding);
		int member_count        = static_cast<int>(buffer_type.member_types.size());

		EC_CORE_TRACE("  {0}", resource.name);
		EC_CORE_TRACE("    Size = {0}", buffer_size);
		EC_CORE_TRACE("    Binding = {0}", binding);
		EC_CORE_TRACE("    Members = {0}", member_count);
	}
}

}  // namespace eclipse