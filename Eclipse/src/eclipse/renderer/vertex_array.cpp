#include "ecpch.h"
#include "vertex_array.h"
#include "renderer.h"
#include "platform/opengl/opengl_vertex_array.h"

namespace eclipse {

ref<VertexArray> VertexArray::create() {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLVertexArray>();
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse