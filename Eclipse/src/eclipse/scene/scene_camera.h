#pragma once

#include "eclipse/renderer/camera.h"
#include "eclipse/common_types/window_size.h"

namespace eclipse {

struct OrthographicProperties {
	float size      = 10.0F;
	float near_clip = -1.0F;
	float far_clip  = 1.0F;
};

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera() = default;
	void set_orthographic(const OrthographicProperties& props);
	void set_viewport_size(const WindowSize& new_size);
	void set_orthographic_size(float size);

	float get_orthographic_size() const { return orthographic_size_; }

private:
	void recalculate_projection();

	float orthographic_size_ = 10.0F;
	float orthographic_near_ = -1.0F;
	float orthographic_far_  = 1.0F;
	float aspect_ratio_      = 1.78F;
};

}  // namespace eclipse