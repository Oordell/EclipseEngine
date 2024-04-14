#include "ecpch.h"
#include "eclipse/renderer/uniform_buffer.h"

#include "eclipse/renderer/renderer.h"
#include "platform/opengl/opengl_uniform_buffer.h"

namespace eclipse {

ref<UniformBuffer> eclipse::UniformBuffer::create(const UniformBufferRecipe& args) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is currently not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLUniformBuffer>(args);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}
}  // namespace eclipse
