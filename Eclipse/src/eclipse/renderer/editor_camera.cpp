#include "ecpch.h"
#include "editor_camera.h"

#include "eclipse/utils/math.h"
#include "eclipse/core/input_manager.h"
#include "eclipse/core/key_codes.h"
#include "eclipse/core/mouse_button_codes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace eclipse {
EditorCamera::EditorCamera(const EditorCameraSpecs& specs)
    : fov_(specs.fov),
      aspect_ratio_(specs.aspect_ratio),
      near_clip_(specs.near_clip),
      far_clip_(specs.far_clip),
      Camera(glm::perspective(specs.fov.in(au::radians), specs.aspect_ratio, specs.near_clip, specs.far_clip)) {
	update_view();
}

void EditorCamera::on_update(au::QuantityF<au::Seconds> ts) {
	if (InputManager::is_key_pressed(KeyCode::left_alt)) {
		const glm::vec2& mouse {InputManager::get_mouse_pose_x().in(units::pixels),
		                        InputManager::get_mouse_pose_y().in(units::pixels)};
		glm::vec2 delta         = (mouse - initial_mouse_position_) * 0.003F;
		initial_mouse_position_ = mouse;

		if (InputManager::is_mouse_button_pressed(MouseCode::button_middle)) {
			mouse_pan(delta);
		} else if (InputManager::is_mouse_button_pressed(MouseCode::button_left)) {
			mouse_rotate(delta);
		} else if (InputManager::is_mouse_button_pressed(MouseCode::button_right)) {
			mouse_zoom(delta.y);
		}
	}

	update_view();
}

void EditorCamera::on_event(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseScrolledEvent>(EC_BIND_EVENT_FN(EditorCamera::on_mouse_scroll));
}

glm::vec3 EditorCamera::get_up_direction() const { return glm::rotate(get_orientation(), glm::vec3(0.0F, 1.0F, 0.0F)); }

glm::vec3 EditorCamera::get_right_direction() const {
	return glm::rotate(get_orientation(), glm::vec3(1.0F, 0.0F, 0.0F));
}

glm::vec3 EditorCamera::get_forward_direction() const {
	return glm::rotate(get_orientation(), glm::vec3(0.0F, 0.0F, -1.0F));
}

glm::quat EditorCamera::get_orientation() const {
	return glm::quat(glm::vec3(-pitch_.in(au::radians), -yaw_.in(au::radians), 0.0F));
}

void EditorCamera::update_projection() {
	aspect_ratio_ = viewport_width_.in<float>(units::pixels) / viewport_height_.in<float>(units::pixels);
	projection_   = glm::perspective(fov_.in(au::radians), aspect_ratio_, near_clip_, far_clip_);
}

void EditorCamera::update_view() {
	// yaw_ = au::radians(0.0F); pitch_ = au::radians(0.0F); // Lock the camera's rotation
	position_             = calculate_position();
	glm::quat orientation = get_orientation();
	view_matrix_          = glm::translate(glm::mat4(1.0F), position_) * glm::toMat4(orientation);
	view_matrix_          = glm::inverse(view_matrix_);
}

bool EditorCamera::on_mouse_scroll(MouseScrolledEvent& e) {
	float delta = e.get_y_offset().in(units::pixels) * 0.1F;
	mouse_zoom(delta);
	update_view();
	return false;
}

void EditorCamera::mouse_pan(const glm::vec2& delta) {
	auto [x_speed, y_speed] = pan_speed();
	focal_point_ += -get_right_direction() * delta.x * x_speed.in(units::pixels_per_seconds) *distance_;
	focal_point_ += get_up_direction() * delta.y * y_speed.in(units::pixels_per_seconds) *distance_;
}

void EditorCamera::mouse_rotate(const glm::vec2& delta) {
	float yaw_sign = get_up_direction().y < 0 ? -1.0F : 1.0F;
	yaw_ += au::radians(yaw_sign * delta.x * rotation_speed());
	pitch_ += au::radians(delta.y * rotation_speed());
}

void EditorCamera::mouse_zoom(float delta) {
	distance_ -= delta * zoom_speed();
	if (distance_ < 1.0F) {
		focal_point_ += get_forward_direction();
		distance_ = 1.0F;
	}
}

glm::vec3 EditorCamera::calculate_position() const { return focal_point_ - get_forward_direction() * distance_; }

ScreenVelocity2D EditorCamera::pan_speed() const {
	float x       = std::min(viewport_width_.in<float>(units::pixels) / 1000.0F, 2.4F);  // max = 2.4f
	float xFactor = 0.0366F * (x * x) - 0.1778F * x + 0.3021F;

	float y       = std::min(viewport_height_.in<float>(units::pixels) / 1000.0F, 2.4F);  // max = 2.4f
	float yFactor = 0.0366F * (y * y) - 0.1778F * y + 0.3021F;

	return {.vel_x = units::pixels_per_seconds(xFactor), .vel_y = units::pixels_per_seconds(yFactor)};
}

float EditorCamera::rotation_speed() const { return 0.8F; }

float EditorCamera::zoom_speed() const {
	float distance = distance_ * 0.2F;
	distance       = std::max(distance, 0.0F);
	float speed    = distance * distance;
	speed          = std::min(speed, 100.0f);  // max speed = 100
	return speed;
}
}  // namespace eclipse