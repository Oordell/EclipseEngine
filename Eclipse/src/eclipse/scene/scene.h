#pragma once

#include "eclipse/core/timestep.h"

#include <entt/entt.hpp>

namespace eclipse {

// Forward declaration
class Entity;

class Scene {
public:
	Scene()  = default;
	~Scene() = default;

	Entity create_entity(const std::string& name = "");

	void on_update(Timestep timestep);

	entt::registry& get_registry() { return registry_; }

private:
	entt::registry registry_;
};

}  // namespace eclipse