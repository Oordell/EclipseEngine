#pragma once

#include "eclipse/core/timestep.h"

#include <entt/entt.hpp>

namespace eclipse {

class Scene {
public:
	Scene()  = default;
	~Scene() = default;

	entt::entity create_entity();

	void on_update(Timestep timestep);

	entt::registry& get_reg() { return registry_; }

private:
	entt::registry registry_;
};

}  // namespace eclipse