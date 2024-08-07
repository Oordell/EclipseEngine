#include "ecpch.h"
#include "shader.h"

#include "renderer.h"
#include "platform/opengl/opengl_shader.h"

namespace eclipse {

ref<Shader> Shader::create(const std::filesystem::path& filepath) { return create_impl(filepath); }

ref<Shader> Shader::create(const ShaderInfo& info) { return create_impl(info); }

template <typename T>
inline ref<Shader> Shader::create_impl(const T& arg) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLShader>(arg);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

void ShaderLibrary::add(const ref<Shader>& shader) {
	auto& name = shader->get_name();
	add(name, shader);
}

void ShaderLibrary::add(const std::string& name, const ref<Shader>& shader) {
	EC_CORE_ASSERT(!exists(name), "Shader already exists");
	shaders_[name] = shader;
}

ref<Shader> ShaderLibrary::load(const std::filesystem::path& filepath) {
	auto shader = Shader::create(filepath);
	add(shader);
	return shader;
}

ref<Shader> ShaderLibrary::load(const std::string& name, const std::filesystem::path& filepath) {
	auto shader = Shader::create(filepath);
	add(name, shader);
	return shader;
}

ref<Shader> ShaderLibrary::get(const std::string& name) {
	EC_CORE_ASSERT(exists(name), "Shader not found!");
	return shaders_[name];
}

bool ShaderLibrary::exists(const std::string& name) const { return shaders_.find(name) != shaders_.end(); }

}  // namespace eclipse