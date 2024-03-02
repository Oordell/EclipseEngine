#pragma once

#include "eclipse/core/core.h"
#include "eclipse/renderer/orthographic_camera.h"
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
	void on_update(au::QuantityF<au::Seconds> timestep);
	void on_event(Event& e);
	void on_resize(const WindowSize& new_size);

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
	au::Quantity<au::Degrees, float> camera_rotation_ {au::degrees(0.0F)};
	au::Quantity<decltype(units::Pixels {} / au::Seconds {}), float> camera_move_speed_ {units::pixels(2.0F) /
	                                                                                     au::seconds(1.0F)};
	au::Quantity<decltype(au::Degrees {} / au::Seconds {}), float> camera_rotate_speed_ {au::degrees(90.0F) /
	                                                                                     au::seconds(1.0F)};
	unsigned int frame_rate_ = 0;
};

}  // namespace eclipse