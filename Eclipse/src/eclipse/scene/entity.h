#pragma once

#include "eclipse/scene/scene.h"
#include <entt/entt.hpp>

namespace eclipse {

class Entity {
public:
	Entity()              = default;
	~Entity()             = default;
	Entity(const Entity&) = default;
	Entity(entt::entity entity, Scene* scene);

	template <typename T, typename... Args>
	T& add_component(Args&&... args) {
		validate_members();
		EC_CORE_ASSERT(!has_component<T>(), "Entity already has this component!");
		return scene_->get_registry().emplace<T>(entity_handle_, std::forward<Args>(args)...);
	}

	template <typename T>
	void remove_component() {
		validate_members();
		EC_CORE_ASSERT(has_component<T>(), "Entity does not have this component!");
		scene_->get_registry().remove<T>(entity_handle_);
	}

	template <typename T>
	T& get_component() {
		validate_members();
		EC_CORE_ASSERT(has_component<T>(), "Entity does not have this component!");
		return scene_->get_registry().get<T>(entity_handle_);
	}

	template <typename T>
	bool has_component() {
		validate_members();
		return scene_->get_registry().try_get<T>(entity_handle_) ? true : false;
	}

	operator bool() { return ((scene_ != nullptr) && (entity_handle_ != entt::null)); }

private:
	void validate_members() const {
		EC_CORE_ASSERT(scene_ != nullptr, "Scene is a null pointer!");
		EC_CORE_ASSERT(entity_handle_ != entt::null, "Entity is not set!");
	}

	entt::entity entity_handle_ {entt::null};
	Scene* scene_ {nullptr};
};

}  // namespace eclipse