#include "ecpch.h"
#include "renderer_api.h"

namespace eclipse {

RendererAPI::API RendererAPI::api_ = RendererAPI::API::open_gl;

}