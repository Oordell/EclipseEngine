#include "ecpch.h"
#include "scene_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eclipse {

SceneCamera::SceneCamera() : Camera() { recalculate_projection(); }

void SceneCamera::set_orthographic(const OrthographicProperties& props) {
	orthographic_size_ = props.size;
	orthographic_near_ = props.near_clip;
	orthographic_far_  = props.far_clip;
	recalculate_projection();
}

void SceneCamera::set_viewport_size(const WindowSize& new_size) {
	aspect_ratio_ = new_size.get_aspect_ratio();
	recalculate_projection();
}

void SceneCamera::set_orthographic_size(float size) {
	orthographic_size_ = size;
	recalculate_projection();
}

void SceneCamera::recalculate_projection() {
	static const float one_half = 0.5F;
	float ortho_left            = -orthographic_size_ * aspect_ratio_ * one_half;
	float ortho_right           = orthographic_size_ * aspect_ratio_ * one_half;
	float ortho_bottom          = -orthographic_size_ * one_half;
	float ortho_top             = orthographic_size_ * one_half;

	projection_ = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, orthographic_near_, orthographic_far_);
}

}  // namespace eclipse