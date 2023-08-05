#include "ecpch.h"
#include "scene.h"
#include "eclipse/scene/components.h"
#include "eclipse/renderer/renderer_2d.h"

#include <glm/glm.hpp>

namespace eclipse {

entt::entity Scene::create_entity() { return registry_.create(); }

void Scene::on_update(Timestep timestep) {
	auto group = registry_.group<component::Transform>(entt::get<component::Color>);
	for (auto entity : group) {
		const auto& [trans, color] = group.get<component::Transform, component::Color>(entity);
		Renderer2D::draw_quad({.transform = trans, .common = {.color = color}});
	}
}

}  // namespace eclipse