#include "ecpch.h"
#include "eclipse/renderer/graphics_context.h"

#include "eclipse/renderer/renderer.h"
#include "platform/opengl/opengl_context.h"

namespace eclipse {

scope<GraphicsContext> GraphicsContext::create(void* window) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_scope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse