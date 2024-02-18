#pragma once

#include "camera.h"
#include "eclipse/core/timestep.h"
#include "eclipse/events/event.h"
#include "eclipse/events/mouse_events.h"
#include "eclipse/common_types/window_size.h"
#include "eclipse/common_types/screen_velocity_2d.h"

#include <glm/glm.hpp>
#include <au.hh>

namespace eclipse {

struct EditorCameraSpecs {
	au::Quantity<au::Degrees, float> fov {};
	float aspect_ratio {};
	float near_clip {};
	float far_clip {};
};

class EditorCamera : public Camera {
public:
	EditorCamera() = default;
	EditorCamera(const EditorCameraSpecs& specs);

	void on_update(Timestep ts);
	void on_event(Event& e);

	inline float get_distance() const { return distance_; }

	inline void set_distance(float d) { distance_ = d; }

	inline void set_viewport_size(const WindowSize& size) {
		viewport_width_  = size.width.in<float>(units::pixels);
		viewport_height_ = size.height.in<float>(units::pixels);
		update_projection();
	}

	const glm::mat4& get_view_matrix() const { return view_matrix_; }

	glm::mat4 get_view_projection() const { return projection_ * view_matrix_; }

	glm::vec3 get_up_direction() const;
	glm::vec3 get_right_direction() const;
	glm::vec3 get_forward_direction() const;

	const glm::vec3& get_position() const { return position_; }

	glm::quat get_orientation() const;

	au::Quantity<au::Radians, float> get_pitch() const { return pitch_; }

	au::Quantity<au::Radians, float> get_yaw() const { return yaw_; }

private:
	void update_projection();
	void update_view();
	bool on_mouse_scroll(MouseScrolledEvent& e);
	void mouse_pan(const glm::vec2& delta);
	void mouse_rotate(const glm::vec2& delta);
	void mouse_zoom(float delta);
	glm::vec3 calculate_position() const;
	ScreenVelocity2D pan_speed() const;
	float rotation_speed() const;
	float zoom_speed() const;

	au::Quantity<au::Degrees, float> fov_ {au::degrees(45.0F)};
	float aspect_ratio_ {1.778F};
	float near_clip_ {0.1F};
	float far_clip_ {1000.0F};

	float distance_ {10.0F};
	float viewport_width_ {1280.0F};
	float viewport_height_ {720.0F};
	au::Quantity<au::Radians, float> pitch_ {au::radians(0.0F)};
	au::Quantity<au::Radians, float> yaw_ {au::radians(0.0F)};

	glm::mat4 view_matrix_ {};
	glm::vec3 position_ {0.0F, 0.0F, 0.0F};
	glm::vec3 focal_point_ {0.0F, 0.0F, 0.0F};
	glm::vec2 initial_mouse_position_ {0.0F, 0.0F};
};

}  // namespace eclipse