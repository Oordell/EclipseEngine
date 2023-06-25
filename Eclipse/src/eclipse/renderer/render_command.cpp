#include "ecpch.h"
#include "render_command.h"

#include "platform/opengl/opengl_renderer_api.h"

namespace eclipse {

scope<RendererAPI> RenderCommand::renderer_api_ = RendererAPI::create();

}