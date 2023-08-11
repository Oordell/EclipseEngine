#pragma once

#include "eclipse/scene/entity.h"

namespace eclipse {

class ScriptableEntity {
public:
	virtual ~ScriptableEntity() = default;

	template <typename T>
	T& get_component() {
		return entity_.get_component<T>();
	}

protected:
	virtual void on_create() {}

	virtual void on_destroy() {}

	virtual void on_update(Timestep timestep) {}

private:
	Entity entity_;
	friend class Scene;
};

}  // namespace eclipse
