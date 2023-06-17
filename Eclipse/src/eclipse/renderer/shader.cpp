#include "ecpch.h"
#include "shader.h"

#include "renderer.h"
#include "platform/opengl/opengl_shader.h"

namespace eclipse {

ref<Shader> Shader::create(const FilePath& filepath) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLShader>(filepath);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

ref<Shader> Shader::create(const ShaderInfo& info) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLShader>(info);
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

ref<Shader> ShaderLibrary::load(const FilePath& filepath) {
	auto shader = Shader::create(filepath);
	add(shader);
	return shader;
}

ref<Shader> ShaderLibrary::load(const std::string& name, const FilePath& filepath) {
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