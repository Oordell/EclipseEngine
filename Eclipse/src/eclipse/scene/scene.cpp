#include "ecpch.h"
#include "scene.h"
#include "eclipse/scene/components.h"
#include "eclipse/renderer/renderer_2d.h"
#include "eclipse/scene/entity.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace eclipse {

static b2BodyType eclipse_rigid_body_2d_type_to_box_2d_type(component::RigidBody2D::BodyType type) {
	using enum component::RigidBody2D::BodyType;
	switch (type) {
		case Static: {
			return b2_staticBody;
		}
		case Dynamic: {
			return b2_dynamicBody;
		}
		case Kinematic: {
			return b2_kinematicBody;
		}
		default: {
			EC_CORE_ASSERT(false, "Wrong type passed!");
			return b2_staticBody;
		}
	}
}

Scene::~Scene() { delete physics_world_; }

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

void Scene::on_update_editor(au::QuantityF<au::Seconds> timestep, EditorCamera& camera) {
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

void Scene::on_update_runtime(au::QuantityF<au::Seconds> timestep) {
	// Update scripts
	registry_.view<component::NativeScript>().each([=](auto entity, auto& nsc) {
		if (nsc.instance == nullptr) {
			nsc.instance          = nsc.instantiate_script_func();
			nsc.instance->entity_ = Entity {entity, this};
			nsc.instance->on_create();
		}
		nsc.instance->on_update(timestep);
	});

	// Physics
	constexpr uint32_t velocity_iterations = 6;
	constexpr uint32_t position_iterations = 2;
	physics_world_->Step(timestep.in(au::seconds), velocity_iterations, position_iterations);

	// Get transform from box2d
	auto view = registry_.view<component::RigidBody2D>();
	for (auto e : view) {
		Entity entity {e, this};
		auto& transform     = entity.get_component<component::Transform>();
		auto& rigid_body_2d = entity.get_component<component::RigidBody2D>();

		b2Body* body            = reinterpret_cast<b2Body*>(rigid_body_2d.runtime_body);
		const auto& position    = body->GetPosition();
		transform.translation.x = position.x;
		transform.translation.y = position.y;
		transform.rotation.z    = body->GetAngle();
	}

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

void Scene::on_runtime_start() {
	physics_world_ = new b2World({0.F, -9.8F});
	auto view      = registry_.view<component::RigidBody2D>();
	for (auto e : view) {
		Entity entity {e, this};
		auto& transform     = entity.get_component<component::Transform>();
		auto& rigid_body_2d = entity.get_component<component::RigidBody2D>();

		b2BodyDef body_def;
		body_def.type = eclipse_rigid_body_2d_type_to_box_2d_type(rigid_body_2d.type);
		body_def.position.Set(transform.translation.x, transform.translation.y);
		body_def.angle = transform.rotation.z;

		b2Body* body = physics_world_->CreateBody(&body_def);
		body->SetFixedRotation(rigid_body_2d.fixed_rotation);
		rigid_body_2d.runtime_body = body;

		if (entity.has_component<component::BoxCollider2D>()) {
			auto& box_collider_2d = entity.get_component<component::BoxCollider2D>();

			b2PolygonShape box_shape;
			box_shape.SetAsBox(box_collider_2d.size.x * transform.scale.x, box_collider_2d.size.y * transform.scale.y);

			b2FixtureDef fixture_def;
			fixture_def.shape                = &box_shape;
			fixture_def.density              = box_collider_2d.density;
			fixture_def.friction             = box_collider_2d.friction;
			fixture_def.restitution          = box_collider_2d.restitution;
			fixture_def.restitutionThreshold = box_collider_2d.restitution_threshold;
			body->CreateFixture(&fixture_def);
		}
	}
}

void Scene::on_runtime_stop() {
	delete physics_world_;
	physics_world_ = nullptr;
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
void Scene::on_component_added<component::RigidBody2D>(Entity entity, component::RigidBody2D& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::BoxCollider2D>(Entity entity, component::BoxCollider2D& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::Camera>(Entity entity, component::Camera& component) {
	if (!(viewport_size_.width.in(units::pixels) > 0 && viewport_size_.height.in(units::pixels) > 0)) {
		EC_CORE_ERROR("Viewport is invalid! Width: {0}, height: {1}", viewport_size_.width, viewport_size_.height);
		return;
	}
	component.camera.set_viewport_size(viewport_size_);
}

}  // namespace eclipse