#include "ecpch.h"
#include "texture.h"

#include "renderer.h"
#include "platform/opengl/opengl_texture.h"

namespace eclipse {

ref<Texture2D> Texture2D::create(const std::string& path) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLTexture2D>(path);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse