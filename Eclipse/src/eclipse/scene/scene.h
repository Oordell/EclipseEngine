#pragma once

#include "eclipse/core/timestep.h"
#include "eclipse/common_types/window_size.h"

#include <entt/entt.hpp>

namespace eclipse {

// Forward declaration
class Entity;

class Scene {
public:
	Scene()  = default;
	~Scene() = default;
	void on_update(Timestep timestep);
	void on_viewport_resize(const WindowSize& new_size);

	Entity create_entity(const std::string& name = "");

	entt::registry& get_registry() { return registry_; }

private:
	entt::registry registry_;
	WindowSize viewport_size_ {};
};

}  // namespace eclipse