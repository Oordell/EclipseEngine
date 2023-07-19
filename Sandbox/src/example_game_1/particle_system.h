#pragma once

#include "eclipse.h"
#include "color.h"

struct ParticleProps {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 velocity_variation;
	ColorRGB color_begin;
	ColorRGB color_end;
	float size_begin;
	float size_end;
	float size_variation;
	float life_time = 1.0F;
};

class ParticleSystem {
public:
	ParticleSystem();
	~ParticleSystem() = default;
	void emit(const ParticleProps& particle_prop);
	void on_update(eclipse::Timestep timestep);
	void on_render();

private:
	struct Particle {
		glm::vec2 position;
		glm::vec2 velocity;
		ColorRGB color_begin;
		ColorRGB color_end;
		float size_begin;
		float size_end;
		float rotation       = 0.0f;
		float life_time      = 1.0f;
		float life_remaining = 0.0f;
		bool active          = false;
	};

	static constexpr uint16_t MAX_PARTICLES = 1000;
	std::vector<Particle> particle_pool_;
	uint32_t pool_index_ = MAX_PARTICLES - 1;
};