#include "ecpch.h"
#include "index_buffer.h"
#include "renderer.h"
#include "platform/opengl/opengl_index_buffer.h"

namespace eclipse {

ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLIndexBuffer>(indices, count);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse