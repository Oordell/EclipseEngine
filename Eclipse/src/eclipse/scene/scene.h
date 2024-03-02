#pragma once

#include "eclipse/common_types/window_size.h"
#include "eclipse/renderer/editor_camera.h"

#include <entt/entt.hpp>

namespace eclipse {

class Entity;

class Scene {
public:
	Scene()  = default;
	~Scene() = default;
	void on_update_editor(au::QuantityF<au::Seconds> timestep, EditorCamera& camera);
	void on_update_runtime(au::QuantityF<au::Seconds> timestep);
	void on_viewport_resize(const WindowSize& new_size);

	Entity create_entity(const std::string& name = "");
	void destroy_entity(Entity entity);

	entt::registry& get_registry() { return registry_; }

	template <typename Component>
	void on_component_added(Entity entity, Component& component);

	[[nodiscard]] Entity get_primary_camera_entity();

private:
	entt::registry registry_;
	WindowSize viewport_size_ {};
};

}  // namespace eclipse