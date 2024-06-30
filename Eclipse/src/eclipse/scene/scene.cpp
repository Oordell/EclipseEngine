#include "ecpch.h"
#include "scene.h"
#include "eclipse/scene/component_concept.h"
#include "eclipse/renderer/renderer_2d.h"
#include "eclipse/scene/entity.h"

#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

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

template <component::IsComponent... Component>
static void copy_component(entt::registry& destination, entt::registry& source,
                           const std::unordered_map<UUID, entt::entity>& entt_map) {
	(
	    [&]() {
		    auto view = source.view<Component>();
		    for (auto e : view) {
			    UUID uuid = source.get<component::ID>(e).id;
			    EC_CORE_ASSERT(entt_map.find(uuid) != entt_map.end(), "Couldn't find entity with uuid: {0} in the map...",
			                   uuid.value());

			    entt::entity dst_entity_id = entt_map.at(uuid);
			    auto& component            = source.get<Component>(e);
			    destination.emplace_or_replace<Component>(dst_entity_id, component);
		    }
	    }(),
	    ...);
}

template <component::IsComponent... Component>
static void copy_component(component::ComponentPack<Component...>, entt::registry& destination, entt::registry& source,
                           const std::unordered_map<UUID, entt::entity>& entt_map) {
	copy_component<Component...>(destination, source, entt_map);
}

template <component::IsComponent... Component>
static void copy_component_if_exists(Entity source, Entity destination) {
	(
	    [&]() {
		    if (source.has_component<Component>()) {
			    destination.add_or_replace_component<Component>(source.get_component<Component>());
		    }
	    }(),
	    ...);
}

template <component::IsComponent... Component>
static void copy_component_if_exists(component::ComponentPack<Component...>, Entity source, Entity destination) {
	copy_component_if_exists<Component...>(source, destination);
}

Scene::~Scene() { delete physics_world_; }

void Scene::on_update_editor(au::QuantityF<au::Seconds> timestep, EditorCamera& camera) { render_scene(camera); }

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

	update_2d_physics(timestep);

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

		draw_sprite_and_circles();

		Renderer2D::end_scene();
	}
}

void Scene::on_update_simulation(au::QuantityF<au::Seconds> timestep, EditorCamera& camera) {
	update_2d_physics(timestep);
	render_scene(camera);
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

void Scene::on_runtime_start() { init_2d_physics(); }

void Scene::on_runtime_stop() { stop_2d_physics(); }

void Scene::on_simulation_start() { init_2d_physics(); }

void Scene::on_simulation_stop() { stop_2d_physics(); }

Entity Scene::create_entity(const std::string& tag) { return create_entity_from_uuid(UUID(), tag); }

Entity Scene::create_entity_from_uuid(const UUID& id, const std::string& tag) {
	Entity e {registry_.create(), this};
	e.add_component<component::ID>(id);
	e.add_component<component::Transform>();
	e.add_component<component::Tag>(tag.empty() ? "Entity" : tag);
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

ref<Scene> Scene::copy(ref<Scene> other) {
	auto new_scene            = make_ref<Scene>();
	new_scene->viewport_size_ = other->viewport_size_;

	auto& src_scene_registry = other->registry_;
	auto& dst_scene_registry = new_scene->registry_;
	std::unordered_map<UUID, entt::entity> entt_map;

	auto id_view = src_scene_registry.view<component::ID>();
	for (auto e : id_view) {
		UUID uuid         = src_scene_registry.get<component::ID>(e).id;
		const auto& tag   = src_scene_registry.get<component::Tag>(e).tag;
		Entity new_entity = new_scene->create_entity_from_uuid(uuid, tag);
		entt_map[uuid]    = static_cast<entt::entity>(new_entity);
	}

	copy_component(component::AllComponents {}, dst_scene_registry, src_scene_registry, entt_map);
	return new_scene;
}

void Scene::duplicate_entity(Entity entity) {
	Entity new_entity = create_entity(entity.get_name());
	copy_component_if_exists(component::AllComponents {}, entity, new_entity);
}

void Scene::init_2d_physics() {
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
			fixture_def.density              = box_collider_2d.density.in(units::densities);
			fixture_def.friction             = box_collider_2d.friction.in(units::newtons);
			fixture_def.restitution          = box_collider_2d.restitution.in(au::unos);
			fixture_def.restitutionThreshold = box_collider_2d.restitution_threshold.in(au::unos);
			body->CreateFixture(&fixture_def);
		}

		if (entity.has_component<component::CircleCollider2D>()) {
			auto& circle_collider_2d = entity.get_component<component::CircleCollider2D>();

			b2CircleShape circle_shape;
			circle_shape.m_p.Set(circle_collider_2d.offset.x, circle_collider_2d.offset.y);
			circle_shape.m_radius = transform.scale.x * circle_collider_2d.radius.in(au::meters);

			b2FixtureDef fixture_def;
			fixture_def.shape                = &circle_shape;
			fixture_def.density              = circle_collider_2d.density.in(units::densities);
			fixture_def.friction             = circle_collider_2d.friction.in(units::newtons);
			fixture_def.restitution          = circle_collider_2d.restitution.in(au::unos);
			fixture_def.restitutionThreshold = circle_collider_2d.restitution_threshold.in(au::unos);
			body->CreateFixture(&fixture_def);
		}
	}
}

void Scene::update_2d_physics(const au::QuantityF<au::Seconds>& timestep) {
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
}

void Scene::stop_2d_physics() {
	delete physics_world_;
	physics_world_ = nullptr;
}

void Scene::render_scene(EditorCamera& camera) {
	Renderer2D::begin_scene(camera);

	draw_sprite_and_circles();

	Renderer2D::end_scene();
}

void Scene::draw_sprite_and_circles() const {
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

	auto circle_view = registry_.view<component::Transform, component::CircleRenderer>();
	for (auto entity : circle_view) {
		const auto& [trans, circle_renderer] = circle_view.get<component::Transform, component::CircleRenderer>(entity);

		Renderer2D::draw_circle(
		    {.transform = trans.get_transform(), .component = circle_renderer, .entity_id = static_cast<int>(entity)});
	}
}

template <typename Component>
void Scene::on_component_added(Entity entity, Component& component) {
	EC_CORE_ASSERT(false, "We need a specialization of \"Scene::on_component_added\"!")
}

template <>
void Scene::on_component_added<component::ID>(Entity entity, component::ID& component) {
	// Do nothing
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
void Scene::on_component_added<component::CircleRenderer>(Entity entity, component::CircleRenderer& component) {
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
void Scene::on_component_added<component::CircleCollider2D>(Entity entity, component::CircleCollider2D& component) {
	// Do nothing
}

template <>
void Scene::on_component_added<component::Camera>(Entity entity, component::Camera& component) {
	if (!(viewport_size_.width.in(units::pixels) > 0 && viewport_size_.height.in(units::pixels) > 0)) {
		EC_CORE_ERROR("Viewport is invalid! Width: {0} px, height: {1} px", viewport_size_.width.in(units::pixels),
		              viewport_size_.height.in(units::pixels));
		return;
	}
	component.camera.set_viewport_size(viewport_size_);
}

}  // namespace eclipse