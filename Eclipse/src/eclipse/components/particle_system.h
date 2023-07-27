#pragma once

#include "eclipse/core/timestep.h"
#include "eclipse/renderer/orthographic_camera.h"

#include <glm/glm.hpp>

namespace eclipse {

struct ParticleProperties {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 velocity_variation;
	glm::vec4 color_begin;
	glm::vec4 color_end;
	float size_begin;
	float size_end;
	float size_variation;
	float life_time = 1.0f;
};

struct Particle {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 color_begin;
	glm::vec4 color_end;
	float rotation = 0.0f;
	float size_begin;
	float size_end;
	float life_time      = 1.0f;
	float life_remaining = 0.0f;
	bool active          = false;
};

class ParticleSystem {
public:
	ParticleSystem(uint32_t max_particles = 1000);
	void on_update(Timestep timestep);
	void on_render(OrthographicCamera& camera);
	void emit(const ParticleProperties& particleProps);

private:
	std::vector<Particle> particle_pool_;
	uint32_t pool_index_;
};

}  // namespace eclipse