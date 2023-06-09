#include "ecpch.h"
#include "renderer_api.h"

#include "platform/opengl/opengl_renderer_api.h"

namespace eclipse {

RendererAPI::API RendererAPI::api_ = RendererAPI::API::open_gl;

scope<RendererAPI> RendererAPI::create() {
	using enum RendererAPI::API;
	switch (api_) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_scope<OpenGLRendererAPI>();
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse