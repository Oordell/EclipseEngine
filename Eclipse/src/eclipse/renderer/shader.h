#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/file_path.h"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace eclipse {

struct ECLIPSE_API ShaderInfo {
	std::string name {};
	std::string vertex_src {};
	std::string fragment_src {};
};

class ECLIPSE_API Shader {
public:
	virtual ~Shader() = default;

	virtual void bind() const                                                = 0;
	virtual void unbind() const                                              = 0;
	virtual void set_mat3(const std::string& name, const glm::mat3& value)   = 0;
	virtual void set_mat4(const std::string& name, const glm::mat4& value)   = 0;
	virtual void set_int(const std::string& name, int value)                 = 0;
	virtual void set_float(const std::string& name, float value)             = 0;
	virtual void set_float2(const std::string& name, const glm::vec2& value) = 0;
	virtual void set_float3(const std::string& name, const glm::vec3& value) = 0;
	virtual void set_float4(const std::string& name, const glm::vec4& value) = 0;
	virtual const std::string& get_name() const                              = 0;

	static ref<Shader> create(const FilePath& filepath);
	static ref<Shader> create(const ShaderInfo& info);
};

class ECLIPSE_API ShaderLibrary {
public:
	void add(const ref<Shader>& shader);
	void add(const std::string& name, const ref<Shader>& shader);
	ref<Shader> load(const FilePath& filepath);
	ref<Shader> load(const std::string& name, const FilePath& filepath);
	ref<Shader> get(const std::string& name);
	bool exists(const std::string& name) const;

private:
	std::unordered_map<std::string, ref<Shader>> shaders_;
};

}  // namespace eclipse