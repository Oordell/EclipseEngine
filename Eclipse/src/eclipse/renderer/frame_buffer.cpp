#include "ecpch.h"
#include "frame_buffer.h"
#include "eclipse/renderer/renderer.h"
#include "platform/opengl/opengl_frame_buffer.h"

namespace eclipse {

ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification& specs) {
	using enum RendererAPI::API;
	switch (Renderer::get_api()) {
		case none: {
			EC_CORE_ASSERT(false, "RendererAPI::none is not supported!");
			return nullptr;
		}
		case open_gl: {
			return make_ref<OpenGLFrameBuffer>(specs);
		}
		default: {
			EC_CORE_ASSERT(false, "Couldn't identify the renderer API type!");
			return nullptr;
		}
	}
}

}  // namespace eclipse