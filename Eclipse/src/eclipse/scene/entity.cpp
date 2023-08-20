#include "ecpch.h"
#include "entity.h"

namespace eclipse {

Entity::Entity(entt::entity entity, Scene* scene) : entity_handle_(entity), scene_(scene) {}

}  // namespace eclipse