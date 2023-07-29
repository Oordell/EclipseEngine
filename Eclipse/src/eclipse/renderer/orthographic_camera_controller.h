#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/orthographic_camera.h"
#include "eclipse/core/timestep.h"
#include "eclipse/events/application_events.h"
#include "eclipse/events/window_events.h"
#include "eclipse/events/mouse_events.h"

namespace eclipse {

struct OrthographicCameraBounds {
	float left {-1.0F};
	float right {1.0F};
	float bottom {-1.0F};
	float top {1.0F};

	float get_width() { return right - left; }

	float get_height() { return top - bottom; }
};

enum class ECLIPSE_API EnableCameraRotation { yes, no };

class ECLIPSE_API OrthographicCameraController {
public:
	OrthographicCameraController(float aspect_ratio, EnableCameraRotation rotate_camera = EnableCameraRotation::no);

	OrthographicCamera& get_camera() { return camera_; }

	const OrthographicCamera& get_camera() const { return camera_; }

	const OrthographicCameraBounds& get_bounds() const { return bounds_; }

	void set_zoom_level(float lvl);
	void on_update(Timestep timestep);
	void on_event(Event& e);

private:
	bool on_mouse_scrolled(MouseScrolledEvent& e);
	bool on_window_resized(WindowResizeEvent& e);
	void set_bounds_and_camera_projection();

	float aspect_ratio_;
	float zoom_level_ {1.0F};
	OrthographicCameraBounds bounds_;
	OrthographicCamera camera_;

	EnableCameraRotation rotate_camera_;
	glm::vec3 camera_position_ {0.0F, 0.0F, 0.0F};
	float camera_rotation_ {0.0F};
	float camera_move_speed_   = 2.0F;   // [units / sec]
	float camera_rotate_speed_ = 90.0F;  // [degrees / sec]
	unsigned int frame_rate_   = 0;
};

}  // namespace eclipse