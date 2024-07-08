#pragma once

#include "eclipse/scene/entity.h"

namespace eclipse {

class EntityDestroyedInterface {
public:
	virtual ~EntityDestroyedInterface()        = default;
	virtual void on_entity_destroyed(Entity e) = 0;
};

}  // namespace eclipse