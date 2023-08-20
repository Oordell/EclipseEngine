#include "ecpch.h"
#include "scene.h"
#include "eclipse/scene/components.h"
#include "eclipse/renderer/renderer_2d.h"
#include "eclipse/scene/entity.h"

#include <glm/glm.hpp>

namespace eclipse {

Entity Scene::create_entity(const std::string& name) {
	Entity e {registry_.create(), this};
	e.add_component<component::Transform>();
	e.add_component<component::Tag>(name.empty() ? "Entity" : name);
	return e;
}

void Scene::destroy_entity(Entity entity) { registry_.destroy(entity); }

void Scene::on_update(Timestep timestep) {
	// Update scripts
	registry_.view<component::NativeScript>().each([=](auto entity, auto& nsc) {
		if (nsc.instance == nullptr) {
			nsc.instance          = nsc.instantiate_script_func();
			nsc.instance->entity_ = Entity {entity, this};
			nsc.instance->on_create();
		}
		nsc.instance->on_update(timestep);
	});

	// Render 2D:
	Camera* main_camera = nullptr;
	glm::mat4 camera_transform;

	auto camera_view = registry_.view<component::Transform, component::Camera>();
	for (auto entity : camera_view) {
		auto [transform, camera] = camera_view.get<component::Transform, component::Camera>(entity);
		if (camera.primary) {
			main_camera      = &camera.camera;
			camera_transform = transform.get_transform();
			break;
		}
	}

	if (main_camera != nullptr) {
		Renderer2D::begin_scene({.projection = main_camera->get_projection(), .transform = camera_transform});

		auto color_group = registry_.view<component::Transform, component::Color>();
		for (auto entity : color_group) {
			const auto& [trans, color] = color_group.get<component::Transform, component::Color>(entity);
			Renderer2D::draw_quad({.transform = trans.get_transform(), .common = {.color = color}});
		}

		auto sprite_group = registry_.view<component::Transform, component::SpriteRenderer>();
		for (auto entity : sprite_group) {
			const auto& [trans, sprite_renderer] = sprite_group.get<component::Transform, component::SpriteRenderer>(entity);
			Renderer2D::draw_quad({.transform = trans.get_transform(), .common = {.color = sprite_renderer.color}});
		}

		Renderer2D::end_scene();
	}
}

void Scene::on_viewport_resize(const WindowSize& new_size) {
	viewport_size_   = new_size;
	auto camera_view = registry_.view<component::Camera>();
	for (auto entity : camera_view) {
		auto& camera = camera_view.get<component::Camera>(entity);
		if (!camera.fixed_aspect_ratio) {
			camera.camera.set_viewport_size(viewport_size_);
		}
	}
}

}  // namespace eclipse