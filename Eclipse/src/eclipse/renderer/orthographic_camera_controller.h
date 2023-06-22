#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/orthographic_camera.h"
#include "eclipse/core/timestep.h"
#include "eclipse/events/application_events.h"
#include "eclipse/events/window_events.h"
#include "eclipse/events/mouse_events.h"

namespace eclipse {

enum class ECLIPSE_API EnableCameraRotation { yes, no };

class ECLIPSE_API OrthographicCameraController {
public:
	OrthographicCameraController(float aspect_ratio, EnableCameraRotation rotate_camera = EnableCameraRotation::no);

	OrthographicCamera& get_camera() { return camera_; }
	const OrthographicCamera& get_camera() const { return camera_; }

	void on_update(Timestep timestep);
	void on_event(Event& e);

private:
	bool on_mouse_scrolled(MouseScrolledEvent& e);
	bool on_window_resized(WindowResizeEvent& e);

	float aspect_ratio_;
	float zoom_level_ {1.0F};
	OrthographicCamera camera_;

	EnableCameraRotation rotate_camera_;
	glm::vec3 camera_position_ {0.0F, 0.0F, 0.0F};
	float camera_rotation_ {0.0F};
	float camera_move_speed_   = 2.0F;   // [units / sec]
	float camera_rotate_speed_ = 90.0F;  // [degrees / sec]
	unsigned int frame_rate_   = 0;
};

}  // namespace eclipse