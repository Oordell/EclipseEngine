#include "ecpch.h"
#include "orthographic_camera_controller.h"
#include "eclipse/core/input_manager.h"
#include "eclipse/core/key_codes.h"
#include "eclipse/utils/math.h"
#include "eclipse/utils/numbers.h"

namespace eclipse {

OrthographicCameraController::OrthographicCameraController(float aspect_ratio, EnableCameraRotation rotate_camera)
    : aspect_ratio_(aspect_ratio),
      bounds_({.left   = -aspect_ratio * zoom_level_,
               .right  = aspect_ratio * zoom_level_,
               .bottom = -zoom_level_,
               .top    = zoom_level_}),
      camera_({.left = bounds_.left, .right = bounds_.right, .bottom = bounds_.bottom, .top = bounds_.top}),
      rotate_camera_(rotate_camera) {}

void OrthographicCameraController::set_zoom_level(float lvl) {
	zoom_level_ = lvl;
	set_bounds_and_camera_projection();
}

void OrthographicCameraController::on_update(au::QuantityF<au::Seconds> timestep) {
	EC_PROFILE_FUNCTION();

	if (InputManager::is_key_pressed(KeyCode::A)) {
		camera_position_.x -= (camera_move_speed_ * timestep).in(units::pixels);
	} else if (InputManager::is_key_pressed(KeyCode::D)) {
		camera_position_.x += (camera_move_speed_ * timestep).in(units::pixels);
	}
	if (InputManager::is_key_pressed(KeyCode::W)) {
		camera_position_.y += (camera_move_speed_ * timestep).in(units::pixels);
	} else if (InputManager::is_key_pressed(KeyCode::S)) {
		camera_position_.y -= (camera_move_speed_ * timestep).in(units::pixels);
	}

	if (rotate_camera_ == EnableCameraRotation::yes) {
		if (InputManager::is_key_pressed(KeyCode::Q)) {
			camera_rotation_ += camera_rotate_speed_ * timestep;
		} else if (InputManager::is_key_pressed(KeyCode::E)) {
			camera_rotation_ -= camera_rotate_speed_ * timestep;
		}

		if (camera_rotation_ > au::degrees(utils::num::DEGREES_180_F)) {
			camera_rotation_ -= 2 * au::degrees(utils::num::DEGREES_180_F);
		} else if (camera_rotation_ <= -au::degrees(utils::num::DEGREES_180_F)) {
			camera_rotation_ += 2 * au::degrees(utils::num::DEGREES_180_F);
		}

		camera_.set_rotation(camera_rotation_);
	}

	camera_.set_position(camera_position_);
}

void OrthographicCameraController::on_event(Event& e) {
	EC_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseScrolledEvent>(EC_BIND_EVENT_FN(OrthographicCameraController::on_mouse_scrolled));
	dispatcher.dispatch<WindowResizeEvent>(EC_BIND_EVENT_FN(OrthographicCameraController::on_window_resized));
}

void OrthographicCameraController::on_resize(const WindowSize& new_size) {
	EC_PROFILE_FUNCTION();

	aspect_ratio_ = new_size.get_aspect_ratio();
	set_bounds_and_camera_projection();
}

bool OrthographicCameraController::on_mouse_scrolled(MouseScrolledEvent& e) {
	EC_PROFILE_FUNCTION();

	zoom_level_ -= e.get_y_offset().in(units::pixels) * 0.1F;
	zoom_level_ = std::max(zoom_level_, 0.25F);
	set_bounds_and_camera_projection();
	return false;
}

bool OrthographicCameraController::on_window_resized(WindowResizeEvent& e) {
	EC_PROFILE_FUNCTION();

	on_resize({.width = e.get_width(), .height = e.get_height()});
	return false;
}

void OrthographicCameraController::set_bounds_and_camera_projection() {
	bounds_ = {.left   = -aspect_ratio_ * zoom_level_,
	           .right  = aspect_ratio_ * zoom_level_,
	           .bottom = -zoom_level_,
	           .top    = zoom_level_};
	camera_.set_projection({.left = bounds_.left, .right = bounds_.right, .bottom = bounds_.bottom, .top = bounds_.top});
}

}  // namespace eclipse