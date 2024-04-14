#pragma once

#include "eclipse/renderer/shader.h"
#include <glm/glm.hpp>

// TODO: Remove!
using GLenum = unsigned int;

namespace eclipse {

class ECLIPSE_API OpenGLShader : public Shader {
public:
	OpenGLShader(const FilePath& filepath);
	OpenGLShader(const ShaderInfo& info);
	virtual ~OpenGLShader();

	void bind() const override;
	void unbind() const override;

	void set_int(const std::string& name, int value) override;
	void set_int_array(const std::string& name, int* values, uint32_t count) override;
	void set_float(const std::string& name, float value) override;
	void set_float2(const std::string& name, const glm::vec2& value) override;
	void set_float3(const std::string& name, const glm::vec3& value) override;
	void set_float4(const std::string& name, const glm::vec4& value) override;
	void set_mat3(const std::string& name, const glm::mat3& value) override;
	void set_mat4(const std::string& name, const glm::mat4& value) override;

	const std::string& get_name() const override { return name_; }

	void upload_uniform_int(const std::string& name, int value);
	void upload_uniform_int_array(const std::string& name, int* values, uint32_t count);
	void upload_uniform_float(const std::string& name, float value);
	void upload_uniform_float2(const std::string& name, const glm::vec2& values);
	void upload_uniform_float3(const std::string& name, const glm::vec3& values);
	void upload_uniform_float4(const std::string& name, const glm::vec4& values);
	void upload_uniform_mat3(const std::string& name, const glm::mat4& matrix);
	void upload_uniform_mat4(const std::string& name, const glm::mat4& matrix);

private:
	std::string read_file(const FilePath& filepath);
	std::unordered_map<GLenum, std::string> pre_process(const std::string& vertes_src);
	void compile_or_get_vulkan_binaries(const std::unordered_map<GLenum, std::string>& shader_sources);
	void compile_or_get_opengl_binaries();
	void create_program();
	void reflect(GLenum stage, const std::vector<uint32_t>& shader_data);
	void init(const std::unordered_map<GLenum, std::string>& sources);

	uint32_t renderer_id_ = 0;
	FilePath file_path_ {""};
	std::string name_ {};

	std::unordered_map<GLenum, std::vector<uint32_t>> vulkan_spirv_;
	std::unordered_map<GLenum, std::vector<uint32_t>> opengl_spirv_;
	std::unordered_map<GLenum, std::string> opengl_source_code_;
};

}  // namespace eclipse