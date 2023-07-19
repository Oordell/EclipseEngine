#include "particle_system.h"
#include "random.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <numbers>

ParticleSystem::ParticleSystem() { particle_pool_.resize(MAX_PARTICLES); }

void ParticleSystem::emit(const ParticleProps& particle_prop) {
	Particle& particle = particle_pool_[pool_index_];
	particle.active    = true;
	particle.position  = particle_prop.position;
	particle.rotation  = Random::get_float() * 2.0F * static_cast<float>(std::numbers::pi);

	// Velocity
	particle.velocity = particle_prop.velocity;
	particle.velocity.x += particle_prop.velocity_variation.x * (Random::get_float() - 0.5F);
	particle.velocity.y += particle_prop.velocity_variation.y * (Random::get_float() - 0.5F);

	// Color
	particle.color_begin = particle_prop.color_begin;
	particle.color_end   = particle_prop.color_end;

	// Size
	particle.size_begin = particle_prop.size_begin + particle_prop.size_variation * (Random::get_float() - 0.5F);
	particle.size_end   = particle_prop.size_end;

	// Life
	particle.life_time      = particle_prop.life_time;
	particle.life_remaining = particle_prop.life_time;

	pool_index_ = --pool_index_ % particle_pool_.size();
}

void ParticleSystem::on_update(eclipse::Timestep timestep) {
	for (auto& particle : particle_pool_) {
		if (!particle.active) {
			continue;
		}

		if (particle.life_remaining <= 0.0F) {
			particle.active = false;
			continue;
		}

		particle.life_remaining -= timestep;
		particle.position += particle.velocity * static_cast<float>(timestep);
		particle.rotation += 0.01F * static_cast<float>(timestep);
	}
}

void ParticleSystem::on_render() {
	for (auto& particle : particle_pool_) {
		if (!particle.active) {
			continue;
		}

		float life      = particle.life_remaining / particle.life_time;
		glm::vec4 color = glm::lerp(particle.color_end.get(), particle.color_begin.get(), life);
		color.a         = color.a * life;

		float size = glm::lerp(particle.size_end, particle.size_begin, life);
		eclipse::Renderer2D::draw_quad(
		    {.position = particle.position, .rotation_deg = particle.rotation, .size = {size, size}, .color = color});
	}
}
