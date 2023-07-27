#pragma once

#include <Eclipse.h>

#include "particle_system.h"

class Player {
public:
	Player();
	~Player() = default;

	void load_asserts();
	void on_update(eclipse::Timestep timestep);
	void on_render();
	void on_imgui_render();
	void reset();

	const glm::vec2& get_position() const { return position_; }

	float get_rotation() { return eclipse::utils::deg_to_rad(velocity_.y * 4.0F - 90.0F); }

	uint32_t get_score() const { return static_cast<uint32_t>((position_.x + 10.0F) / 10.0F); }

private:
	glm::vec2 position_ = {-10.0f, 0.0f};
	glm::vec2 velocity_ = {5.0f, 0.0f};

	float engine_power_         = 0.5F;
	float gravity_              = 0.4F;
	float time_                 = 0.0F;
	float smoke_emit_interval_  = 0.4F;
	float smoke_next_emit_time_ = smoke_emit_interval_;

	ParticleProps smoke_particle_;
	ParticleProps engine_particle_;
	ParticleSystem particle_system_;

	eclipse::ref<eclipse::Texture2D> ship_texture_;
};