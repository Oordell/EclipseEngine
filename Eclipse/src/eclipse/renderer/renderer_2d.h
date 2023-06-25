#pragma once

#include "eclipse/core/core.h"
#include "orthographic_camera.h"

namespace eclipse {

struct QuadMetaDataPosition2D {
	glm::vec2 position {0.0F, 0.0F};
	float rotation_deg {0.0F};
	glm::vec2 size {0.0F, 0.0F};
	glm::vec4 color {0.0F, 0.0F, 0.0F, 0.0F};
};

struct QuadMetaDataPosition3D {
	glm::vec3 position {0.0F, 0.0F, 0.0F};
	float rotation_deg {0.0F};
	glm::vec2 size {0.0F, 0.0F};
	glm::vec4 color {0.0F, 0.0F, 0.0F, 0.0F};
};

class ECLIPSE_API Renderer2D {
public:
	static void init();
	static void shutdown();
	static void begin_scene(const OrthographicCamera& camera);
	static void end_scene();

	static void draw_quad(const QuadMetaDataPosition2D& info);
	static void draw_quad(const QuadMetaDataPosition3D& info);
};

}  // namespace eclipse