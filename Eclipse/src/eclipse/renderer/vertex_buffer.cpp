#include "ecpch.h"
#include "vertex_buffer.h"
#include "renderer.h"
#include "platform/opengl/opengl_vertex_buffer.h"

namespace eclipse {

ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLVertexBuffer>(vertices, size);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse