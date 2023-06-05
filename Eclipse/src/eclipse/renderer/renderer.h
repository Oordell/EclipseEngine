#pragma once

#include "eclipse/core.h"
#include "render_command.h"
#include "orthographic_camera.h"
#include "shader.h"

namespace eclipse {

class ECLIPSE_API Renderer {
public:
	static void begin_scene(OrthographicCamera& camera);

	static void end_scene();

	static void submit(const ref<Shader>& shader, const ref<VertexArray>& vertex_array,
	                   const glm::mat4& transform = glm::mat4(1.0F));

	inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }

private:
	struct SceneData {
		glm::mat4 view_projection_matrix {};
	};

	static SceneData* scene_data_;
};
}  // namespace eclipse