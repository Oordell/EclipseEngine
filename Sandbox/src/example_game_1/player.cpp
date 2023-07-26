#include "player.h"

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

Player::Player() {
	// Smoke
	smoke_particle_.position           = {0.0F, 0.0F};
	smoke_particle_.velocity           = {-2.0F, 0.0F};
	smoke_particle_.velocity_variation = {4.0F, 2.0F};
	smoke_particle_.size_begin         = 0.35F;
	smoke_particle_.size_end           = 0.0F;
	smoke_particle_.size_variation     = 0.15F;
	smoke_particle_.color_begin        = {.red = 0.8F, .green = 0.8F, .blue = 0.8F, .alpha = 1.0F};
	smoke_particle_.color_end          = {.red = 0.6F, .green = 0.6F, .blue = 0.6F, .alpha = 1.0F};
	smoke_particle_.life_time          = 4.0F;

	// Flames
	static const float MAX_COLOR_VALUE  = 255.0F;
	engine_particle_.position           = {0.0F, 0.0F};
	engine_particle_.velocity           = {-2.0F, 0.0F};
	engine_particle_.velocity_variation = {3.0F, 1.0F};
	engine_particle_.size_begin         = 0.5F;
	engine_particle_.size_end           = 0.0F;
	engine_particle_.size_variation     = 0.3F;
	engine_particle_.life_time          = 1.0F;
	/* clang-format off */
	engine_particle_.color_begin        = {
	    .red = 254 / MAX_COLOR_VALUE, 
		.green = 109 / MAX_COLOR_VALUE, 
		.blue = 41 / MAX_COLOR_VALUE, 
		.alpha = 1.0F
	};
	engine_particle_.color_end = {
	    .red = 254 / MAX_COLOR_VALUE, 
		.green = 212 / MAX_COLOR_VALUE, 
		.blue = 123 / MAX_COLOR_VALUE, 
		.alpha = 1.0F
	};
	/* clang-format on */
}

void Player::load_asserts() { ship_texture_ = eclipse::Texture2D::create("assets/textures/ship.png"); }

void Player::on_update(eclipse::Timestep timestep) {
	time_ += timestep;

	if (eclipse::InputManager::is_key_pressed(EC_KEY_SPACE)) {
		velocity_.y += engine_power_;
		if (velocity_.y < 0.0F) {
			velocity_.y += engine_power_ * 2.0F;
		}

		// Flames
		glm::vec2 emission_point = {0.0F, -0.6F};
		float rotation           = static_cast<float>(eclipse::deg_to_rad(get_rotation()));
		glm::vec4 rotated =
		    glm::rotate(glm::mat4(1.0F), rotation, {0.0F, 0.0F, 1.0F}) * glm::vec4(emission_point, 0.0F, 1.0F);
		engine_particle_.position   = position_ + glm::vec2 {rotated.x, rotated.y};
		engine_particle_.velocity.y = -velocity_.y * 0.2F - 0.2F;
		particle_system_.emit(engine_particle_);
	} else {
		velocity_.y -= gravity_;
	}

	velocity_.y = glm::clamp(velocity_.y, -20.0F, 20.0F);
	position_ += velocity_ * static_cast<float>(timestep);

	// Particles
	if (time_ > smoke_next_emit_time_) {
		smoke_particle_.position = position_;
		particle_system_.emit(smoke_particle_);
		smoke_next_emit_time_ += smoke_emit_interval_;
	}

	particle_system_.on_update(timestep);
}

void Player::on_render() {
	particle_system_.on_render();
	eclipse::Renderer2D::draw_quad({.position     = {position_.x, position_.y, 0.5F},
	                                .rotation_rad = get_rotation(),
	                                .size         = {1.0F, 1.3F},
	                                .texture      = ship_texture_});
}

void Player::on_imgui_render() {
	ImGui::DragFloat("Engine Power", &engine_power_, 0.1F);
	ImGui::DragFloat("Gravity", &gravity_, 0.1F);
}

void Player::reset() {
	position_ = {-10.0F, 0.0F};
	velocity_ = {5.0F, 0.0F};
}
