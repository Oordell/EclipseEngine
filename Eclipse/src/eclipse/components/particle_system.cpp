#include "ecpch.h"
#include "particle_system.h"
#include "eclipse/utils/random.h"
#include "eclipse/renderer/renderer_2d.h"

#include <glm/gtx/compatibility.hpp>

namespace eclipse {

ParticleSystem::ParticleSystem(uint32_t max_particles) {
	if (max_particles < 1) {
		max_particles = 1;
	}
	particle_pool_.resize(max_particles);
	pool_index_ = max_particles - 1;
}

void ParticleSystem::on_update(Timestep timestep) {
	for (auto& particle : particle_pool_) {
		if (!particle.active) {
			continue;
		}

		if (particle.life_remaining <= 0.0f) {
			particle.active = false;
			continue;
		}

		particle.life_remaining -= timestep;
		particle.position += particle.velocity * static_cast<float>(timestep);
		particle.rotation += 0.01f * timestep;
	}
}

void ParticleSystem::on_render(OrthographicCamera& camera) {
	for (auto& particle : particle_pool_) {
		if (!particle.active) {
			continue;
		}

		// Fade away particles
		float life      = particle.life_remaining / particle.life_time;
		glm::vec4 color = glm::lerp(particle.color_end, particle.color_begin, life);

		float size = glm::lerp(particle.size_end, particle.size_begin, life);
		// Render
		Renderer2D::draw_quad({.position     = {particle.position.x, particle.position.y, 0.0F},
		                       .rotation_rad = particle.rotation,
		                       .size         = {size, size},
		                       .color        = color});
	}
}

void ParticleSystem::emit(const ParticleProperties& particle_props) {
	Particle& particle = particle_pool_[pool_index_];
	particle.active    = true;
	particle.position  = particle_props.position;
	particle.rotation  = utils::Random::get_normalized_float() * 2.0f * glm::pi<float>();

	// Velocity
	particle.velocity = particle_props.velocity;
	particle.velocity.x += particle_props.velocity_variation.x * (utils::Random::get_normalized_float() - 0.5f);
	particle.velocity.y += particle_props.velocity_variation.y * (utils::Random::get_normalized_float() - 0.5f);

	// Color
	particle.color_begin = particle_props.color_begin;
	particle.color_end   = particle_props.color_end;

	particle.life_time      = particle_props.life_time;
	particle.life_remaining = particle_props.life_time;
	particle.size_begin =
	    particle_props.size_begin + particle_props.size_variation * (utils::Random::get_normalized_float() - 0.5f);
	particle.size_end = particle_props.size_end;

	pool_index_ = --pool_index_ % particle_pool_.size();
}

}  // namespace eclipse