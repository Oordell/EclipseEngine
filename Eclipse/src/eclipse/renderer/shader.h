#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/strong_types.h"

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

	virtual void bind() const                   = 0;
	virtual void unbind() const                 = 0;
	virtual const std::string& get_name() const = 0;

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