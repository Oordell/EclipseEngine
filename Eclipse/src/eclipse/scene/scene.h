#pragma once

#include "eclipse/common_types/window_size.h"
#include "eclipse/renderer/editor_camera.h"
#include "eclipse/core/uuid.h"

#include <entt/entt.hpp>

class b2World;

namespace eclipse {

class Entity;

class Scene {
public:
	Scene() = default;
	~Scene();
	void on_update_editor(au::QuantityF<au::Seconds> timestep, EditorCamera& camera);
	void on_update_runtime(au::QuantityF<au::Seconds> timestep);
	void on_viewport_resize(const WindowSize& new_size);
	void on_runtime_start();
	void on_runtime_stop();

	Entity create_entity(const std::string& tag = "");
	Entity create_entity_from_uuid(const UUID& id, const std::string& tag = "");
	void destroy_entity(Entity entity);

	entt::registry& get_registry() { return registry_; }

	template <typename Component>
	void on_component_added(Entity entity, Component& component);

	[[nodiscard]] Entity get_primary_camera_entity();

	[[nodiscard]] static ref<Scene> copy(ref<Scene> other);

	void duplicate_entity(Entity entity);

private:
	entt::registry registry_;
	WindowSize viewport_size_ {};
	b2World* physics_world_ = nullptr;
};

}  // namespace eclipse