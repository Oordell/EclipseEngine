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
	void on_update_simulation(au::QuantityF<au::Seconds> timestep, EditorCamera& camera);
	void on_viewport_resize(const WindowSize& new_size);
	void on_runtime_start();
	void on_runtime_stop();
	void on_simulation_start();
	void on_simulation_stop();
	Entity create_entity(const std::string& tag = "");
	Entity create_entity_from_uuid(const UUID& id, const std::string& tag = "");
	void destroy_entity(Entity entity);
	[[nodiscard]] Entity get_primary_camera_entity();
	[[nodiscard]] static ref<Scene> copy(ref<Scene> other);
	void duplicate_entity(Entity entity);

	entt::registry& get_registry() { return registry_; }

	template <typename... Components>
	auto get_view_of_all_entities_of_type() {
		return registry_.view<Components...>();
	}

	template <typename Component>
	void on_component_added(Entity entity, Component& component);

private:
	void init_2d_physics();
	void update_2d_physics(const au::QuantityF<au::Seconds>& timestep);
	void stop_2d_physics();
	void render_scene(EditorCamera& camera);
	void draw_sprite_and_circles() const;

	entt::registry registry_;
	WindowSize viewport_size_ {};
	b2World* physics_world_ = nullptr;
};

}  // namespace eclipse