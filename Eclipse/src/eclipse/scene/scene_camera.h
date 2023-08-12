#pragma once

#include "eclipse/renderer/camera.h"
#include "eclipse/common_types/window_size.h"
#include "eclipse/utils/math.h"

namespace eclipse {

enum class ProjectionType : uint16_t { perspective = 0, orthographic = 1 };
static constexpr uint16_t NUM_OF_PROJECTION_TYPES = 2;

struct OrthographicProperties {
	float size      = 10.0F;
	float near_clip = -1.0F;
	float far_clip  = 1.0F;
};

struct PerspectiveProperties {
	float fov       = utils::deg_to_rad(45.0F);
	float near_clip = 0.01F;
	float far_clip  = 1000.0F;
};

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera() = default;
	void set_orthographic(const OrthographicProperties& props);
	void set_perspective(const PerspectiveProperties& props);
	void set_viewport_size(const WindowSize& new_size);
	void set_orthographic_size(float size);
	void set_orthographic_near_clip(float near_clip);
	void set_orthographic_far_clip(float far_clip);
	void set_perspective_vertical_fov(float fov_rad);
	void set_perspective_near_clip(float near_clip);
	void set_perspective_far_clip(float far_clip);

	float get_orthographic_size() const { return orthographic_size_; }

	float get_orthographic_near_clip() const { return orthographic_near_; }

	float get_orthographic_far_clip() const { return orthographic_far_; }

	float get_perspective_vertical_fov() const { return perspective_fov_; }

	float get_perspective_near_clip() const { return perspective_near_; }

	float get_perspective_far_clip() const { return perspective_far_; }

	ProjectionType get_projection_type() const { return projection_type_; }

	void set_projection_type(ProjectionType type) { projection_type_ = type; }

private:
	void recalculate_projection();

	ProjectionType projection_type_ = ProjectionType::orthographic;
	float orthographic_size_        = 10.0F;
	float orthographic_near_        = -1.0F;
	float orthographic_far_         = 1.0F;
	float perspective_fov_          = utils::deg_to_rad(45.0F);
	float perspective_near_         = 0.01F;
	float perspective_far_          = 1000.0F;
	float aspect_ratio_             = 1.78F;
};

}  // namespace eclipse