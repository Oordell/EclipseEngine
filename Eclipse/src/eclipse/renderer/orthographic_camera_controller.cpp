#include "ecpch.h"
#include "orthographic_camera_controller.h"
#include "eclipse/core/input_manager.h"
#include "eclipse/core/key_codes.h"

namespace eclipse {

OrthographicCameraController::OrthographicCameraController(float aspect_ratio, EnableCameraRotation rotate_camera)
    : aspect_ratio_(aspect_ratio),
      camera_({.left   = -aspect_ratio * zoom_level_,
               .right  = aspect_ratio * zoom_level_,
               .bottom = -zoom_level_,
               .top    = zoom_level_}),
      rotate_camera_(rotate_camera) {}

void OrthographicCameraController::on_update(Timestep timestep) {
	if (InputManager::is_key_pressed(EC_KEY_A)) {
		camera_position_.x -= camera_move_speed_ * timestep;
	} else if (InputManager::is_key_pressed(EC_KEY_D)) {
		camera_position_.x += camera_move_speed_ * timestep;
	}
	if (InputManager::is_key_pressed(EC_KEY_W)) {
		camera_position_.y += camera_move_speed_ * timestep;
	} else if (InputManager::is_key_pressed(EC_KEY_S)) {
		camera_position_.y -= camera_move_speed_ * timestep;
	}
	if (rotate_camera_ == EnableCameraRotation::yes) {
		if (InputManager::is_key_pressed(EC_KEY_Q)) {
			camera_rotation_ += camera_rotate_speed_ * timestep;
		} else if (InputManager::is_key_pressed(EC_KEY_E)) {
			camera_rotation_ -= camera_rotate_speed_ * timestep;
		}

		camera_.set_rotation(camera_rotation_);
	}

	camera_.set_position(camera_position_);
}

void OrthographicCameraController::on_event(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.dispatch<MouseScrolledEvent>(EC_BIND_EVENT_FN(OrthographicCameraController::on_mouse_scrolled));
	dispatcher.dispatch<WindowResizeEvent>(EC_BIND_EVENT_FN(OrthographicCameraController::on_window_resized));
}

bool OrthographicCameraController::on_mouse_scrolled(MouseScrolledEvent& e) {
	zoom_level_ -= e.get_y_offset() * 0.1F;
	zoom_level_ = std::max(zoom_level_, 0.25F);
	camera_.set_projection({.left   = -aspect_ratio_ * zoom_level_,
	                        .right  = aspect_ratio_ * zoom_level_,
	                        .bottom = -zoom_level_,
	                        .top    = zoom_level_});

	return false;
}

bool OrthographicCameraController::on_window_resized(WindowResizeEvent& e) {
	aspect_ratio_ = static_cast<float>(e.get_width()) / static_cast<float>(e.get_height());
	camera_.set_projection({.left   = -aspect_ratio_ * zoom_level_,
	                        .right  = aspect_ratio_ * zoom_level_,
	                        .bottom = -zoom_level_,
	                        .top    = zoom_level_});
	return false;
}

}  // namespace eclipse