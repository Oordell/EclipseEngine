#include "ecpch.h"
#include "scene.h"
#include "eclipse/scene/components.h"
#include "eclipse/renderer/renderer_2d.h"
#include "eclipse/scene/entity.h"

#include <glm/glm.hpp>

namespace eclipse {

Entity Scene::create_entity(const std::string& name) {
	Entity e {registry_.create(), ref<Scene>(this)};
	e.add_component<component::Transform>();
	e.add_component<component::Tag>(name.empty() ? "Entity" : name);
	return e;
}

void Scene::on_update(Timestep timestep) {
	Camera* main_camera         = nullptr;
	glm::mat4* camera_transform = nullptr;

	auto camera_group = registry_.view<component::Transform, component::Camera>();
	for (auto entity : camera_group) {
		const auto& [transform, camera] = camera_group.get<component::Transform, component::Camera>(entity);
		if (camera.primary) {
			main_camera      = &camera.camera;
			camera_transform = &transform.transform;
			break;
		}
	}

	if (main_camera != nullptr) {
		Renderer2D::begin_scene({.projection = main_camera->get_projection(), .transform = *camera_transform});
		
		auto group = registry_.view<component::Transform, component::Color>();
		for (auto entity : group) {
			const auto& [trans, color] = group.get<component::Transform, component::Color>(entity);
			Renderer2D::draw_quad({.transform = trans, .common = {.color = color}});
		}

		Renderer2D::end_scene();
	}
}

}  // namespace eclipse