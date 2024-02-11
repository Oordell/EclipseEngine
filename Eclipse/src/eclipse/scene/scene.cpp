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

Entity Scene::get_primary_camera_entity() {
	auto view = registry_.view<component::Camera>();
	for (auto entity : view) {
		const auto& camera = view.get<component::Camera>(entity);
		if (camera.primary) {
			return {entity, this};
		}
	}
	return {};
}

void Scene::on_update_editor(Timestep timestep, EditorCamera& camera) {
	Renderer2D::begin_scene(camera);

	auto color_group = registry_.view<component::Transform, component::Color>();
	for (auto entity : color_group) {
		const auto& [trans, color] = color_group.get<component::Transform, component::Color>(entity);
		//	Renderer2D::draw_quad({.transform = trans.get_transform(), .common = {.color = color}});
		Renderer2D::draw_sprite({.transform = trans.get_transform(),
		                         .component = component::SpriteRenderer(color.color),
		                         .entity_id = static_cast<int>(entity)});
	}

	auto sprite_group = registry_.view<component::Transform, component::SpriteRenderer>();
	for (auto entity : sprite_group) {
		const auto& [trans, sprite_renderer] = sprite_group.get<component::Transform, component::SpriteRenderer>(entity);
		Renderer2D::draw_sprite(
		    {.transform = trans.get_transform(), .component = sprite_renderer, .entity_id = static_cast<int>(entity)});
	}

	Renderer2D::end_scene();
}

void Scene::on_update_runtime(Timestep timestep) {
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
			//	Renderer2D::draw_quad({.transform = trans.get_transform(), .common = {.color = color}});
			Renderer2D::draw_sprite({.transform = trans.get_transform(),
			                         .component = component::SpriteRenderer(color.color),
			                         .entity_id = static_cast<int>(entity)});
		}

		auto sprite_group = registry_.view<component::Transform, component::SpriteRenderer>();
		for (auto entity : sprite_group) {
			const auto& [trans, sprite_renderer] = sprite_group.get<component::Transform, component::SpriteRenderer>(entity);
			Renderer2D::draw_sprite(
			    {.transform = trans.get_transform(), .component = sprite_renderer, .entity_id = static_cast<int>(entity)});
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

template <typename Component>
void Scene::on_component_added(Entity entity, Component& component) {
	EC_CORE_ASSERT(false, "We need a specialization of \"Scene::on_component_added\"!")
}

template <>
void Scene::on_component_added<component::Tag>(Entity entity, component::Tag& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::Transform>(Entity entity, component::Transform& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::Color>(Entity entity, component::Color& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::SpriteRenderer>(Entity entity, component::SpriteRenderer& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::Camera>(Entity entity, component::Camera& component) {
	component.camera.set_viewport_size(viewport_size_);
}

}  // namespace eclipse