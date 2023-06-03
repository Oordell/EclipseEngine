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

	static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertex_array);

	inline static RendererAPI::API get_api() { return RendererAPI::get_api(); }

private:
	struct SceneData {
		glm::mat4 view_projection_matrix {};
	};

	static SceneData* scene_data_;
};
}  // namespace eclipse