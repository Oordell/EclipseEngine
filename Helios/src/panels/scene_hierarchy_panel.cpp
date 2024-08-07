#include "ecpch.h"
#include "scene_hierarchy_panel.h"
#include "content_browser_panel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <au.hh>
#include <concepts>

namespace eclipse {

SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) : EntityDestroyedInterface() {
	set_context(context);
}

void SceneHierarchyPanel::on_entity_destroyed(Entity e) {
	if (selection_context_ == e) {
		selection_context_ = {};
	}
}

void SceneHierarchyPanel::set_context(const ref<Scene>& context) {
	context_           = context;
	selection_context_ = {};
}

void SceneHierarchyPanel::on_imgui_render() {
	ImGui::Begin("Scene Hierarchy");

	context_->get_registry().each([&](auto entity_id) {
		Entity entity {entity_id, context_.get()};
		draw_entity_node(entity);
	});

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		selection_context_ = {};
	}

	// Right click on blank space
	ImGuiPopupFlags flags = ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight;
	if (ImGui::BeginPopupContextWindow(0, flags)) {
		if (ImGui::MenuItem("Create empty entity")) {
			context_->create_entity("Empty entity");
		}
		ImGui::EndPopup();
	}
	ImGui::End();

	ImGui::Begin("Properties");
	if (selection_context_) {
		draw_components(selection_context_);
	}
	ImGui::End();
}

void SceneHierarchyPanel::set_selected_entity(Entity entity) { selection_context_ = entity; }

void SceneHierarchyPanel::draw_entity_node(Entity entity) {
	// Filter away entities we don't want to display here:
	if (entity.has_component<component::TextureSheetComponent>()) {
		return;
	}
	auto& tag_component = entity.get_component<component::Tag>();
	uint64_t entity_id  = static_cast<uint64_t>(entity);

	ImGuiTreeNodeFlags flags =
	    ((selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(entity_id), flags, tag_component.tag.c_str());
	if (ImGui::IsItemClicked()) {
		selection_context_ = entity;
	}

	bool entity_deleted = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete entity")) {
			entity_deleted = true;
		}
		ImGui::EndPopup();
	}

	if (opened) {
		ImGui::TreePop();
	}

	if (entity_deleted) {
		if (selection_context_ == entity) {
			selection_context_ = {};
		}
		context_->destroy_entity(entity);
	}
}

void SceneHierarchyPanel::draw_components(Entity entity) {
	draw_tag_component(entity);
	draw_transform_component(entity);
	draw_camera_component(entity);
	draw_color_component(entity);
	draw_sprite_renderer_component(entity);
	draw_circle_renderer_component(entity);
	draw_rigid_body_2d_component(entity);
	draw_box_collider_2d_component(entity);
	draw_circle_collider_2d_component(entity);
	draw_sub_texture_component(entity);
}

void SceneHierarchyPanel::draw_tag_component(Entity entity) {
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4.0F, 4.0F});
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);

	if (entity.has_component<component::Tag>()) {
		auto& tag = entity.get_component<component::Tag>().tag;

		static char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
			tag = std::string(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	panel_utils::draw_button("Add Component", {}, []() { ImGui::OpenPopup("AddComponent"); });

	if (ImGui::BeginPopup("AddComponent")) {
		add_pop_up_option<component::Camera>(entity, "Camera");
		add_pop_up_option<component::SpriteRenderer>(entity, "Sprite Renderer");
		add_pop_up_option<component::CircleRenderer>(entity, "Circle Renderer");
		add_pop_up_option<component::SubTexture>(entity, "Sub Texture");
		add_pop_up_option<component::Color>(entity, "Color");
		add_pop_up_option<component::RigidBody2D>(entity, "Rigid Body 2D");
		add_pop_up_option<component::BoxCollider2D>(entity, "Box Collider 2D");
		add_pop_up_option<component::CircleCollider2D>(entity, "Circle Collider 2D");
		ImGui::EndPopup();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar(2);
}

template <component::IsComponent Component>
void SceneHierarchyPanel::add_pop_up_option(Entity entity, const std::string& menu_item) {
	if (!entity.has_component<Component>() && ImGui::MenuItem(menu_item.c_str())) {
		selection_context_.add_component<Component>();
		ImGui::CloseCurrentPopup();
	}
}

void SceneHierarchyPanel::draw_transform_component(Entity entity) {
	panel_utils::draw_component<component::Transform>("Transform", entity, [this](auto& component) {
		panel_utils::draw_drag_float(
		    {.label = "Translation",
		     .drag_float_settings =
		         {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED, .button_text = "X"},
		           .drag_float_settings = {.drag_float_tag = "##X", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN, .button_text = "Y"},
		           .drag_float_settings = {.drag_float_tag = "##Y", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_BLUE, .button_text = "Z"},
		           .drag_float_settings = {.drag_float_tag = "##Z", .value_speed = 0.1F}}}},
		    component.translation);

		glm::vec3 rotation = glm::degrees(component.rotation);
		panel_utils::draw_drag_float(
		    {.label = "Rotation",
		     .drag_float_settings =
		         {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED, .button_text = "X"},
		           .drag_float_settings = {.drag_float_tag = "##X", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN, .button_text = "Y"},
		           .drag_float_settings = {.drag_float_tag = "##Y", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_BLUE, .button_text = "Z"},
		           .drag_float_settings = {.drag_float_tag = "##Z", .value_speed = 0.1F}}}},
		    rotation);
		component.rotation = glm::radians(rotation);

		panel_utils::draw_drag_float(
		    {.label               = "Scale",
		     .drag_float_settings = {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED,
		                                                      .reset_value   = 1.0F,
		                                                      .button_text   = "X"},
		                              .drag_float_settings = {.drag_float_tag = "##X", .value_speed = 0.1F}},
		                             {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN,
		                                                      .reset_value   = 1.0F,
		                                                      .button_text   = "Y"},
		                              .drag_float_settings = {.drag_float_tag = "##Y", .value_speed = 0.1F}},
		                             {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_BLUE,
		                                                      .reset_value   = 1.0F,
		                                                      .button_text   = "Z"},
		                              .drag_float_settings = {.drag_float_tag = "##Z", .value_speed = 0.1F}}}},
		    component.scale);
	});
}

void SceneHierarchyPanel::draw_camera_component(Entity entity) {
	panel_utils::draw_component<component::Camera>("Camera", entity, [this](auto& component) {
		auto& camera = component.camera;

		ImGui::Checkbox("Primary", &component.primary);

		const char* projection_type_string[] = {"Perspective", "Orthographic"};
		const char* current_projection_type_string =
		    projection_type_string[static_cast<uint16_t>(camera.get_projection_type())];
		if (ImGui::BeginCombo("Projection", current_projection_type_string)) {
			for (uint16_t i = 0; i < NUM_OF_PROJECTION_TYPES; i++) {
				bool is_selected = current_projection_type_string == projection_type_string[i];
				if (ImGui::Selectable(projection_type_string[i], is_selected)) {
					current_projection_type_string = projection_type_string[i];
					camera.set_projection_type(static_cast<ProjectionType>(i));
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		if (camera.get_projection_type() == ProjectionType::perspective) {
			float perspective_fov       = camera.get_perspective_vertical_fov().in(au::degrees);
			float perspective_near_clip = camera.get_perspective_near_clip();
			float perspective_far_clip  = camera.get_perspective_far_clip();

			panel_utils::draw_drag_float(
			    {.label               = "Vertical FOV",
			     .drag_float_settings = {{.button_settings     = {.reset_value = 45.F},
			                              .drag_float_settings = {.value_speed = 0.1F, .value_min = 0.F, .value_max = 180.F}}}},
			    perspective_fov);
			panel_utils::draw_drag_float(
			    {.label               = "Near",
			     .drag_float_settings = {{.button_settings = {.reset_value = 0.01F}, .drag_float_settings = {}}}},
			    perspective_near_clip);
			panel_utils::draw_drag_float(
			    {.label               = "Far",
			     .drag_float_settings = {{.button_settings = {.reset_value = 1000.F}, .drag_float_settings = {}}}},
			    perspective_far_clip);

			camera.set_perspective_vertical_fov(au::degrees(perspective_fov).as(au::radians));
			camera.set_perspective_near_clip(perspective_near_clip);
			camera.set_perspective_far_clip(perspective_far_clip);
		}

		if (camera.get_projection_type() == ProjectionType::orthographic) {
			float ortho_size      = camera.get_orthographic_size();
			float ortho_near_clip = camera.get_orthographic_near_clip();
			float ortho_far_clip  = camera.get_orthographic_far_clip();

			panel_utils::draw_drag_float(
			    {.label               = "Size",
			     .drag_float_settings = {{.button_settings = {.reset_value = 10.F}, .drag_float_settings = {}}}},
			    ortho_size);
			panel_utils::draw_drag_float(
			    {.label               = "Near",
			     .drag_float_settings = {{.button_settings = {.reset_value = -1.F}, .drag_float_settings = {}}}},
			    ortho_near_clip);
			panel_utils::draw_drag_float(
			    {.label = "Far", .drag_float_settings = {{.button_settings = {.reset_value = 1.F}, .drag_float_settings = {}}}},
			    ortho_far_clip);

			camera.set_orthographic_size(ortho_size);
			camera.set_orthographic_near_clip(ortho_near_clip);
			camera.set_orthographic_far_clip(ortho_far_clip);
		}

		ImGui::Checkbox("Fixed Aspect Ratio", &component.fixed_aspect_ratio);
	});
}

void SceneHierarchyPanel::draw_color_component(Entity entity) {
	panel_utils::draw_component<component::Color>(
	    "Color", entity, [](auto& component) { ImGui::ColorEdit4("Color", glm::value_ptr(component.color)); });
}

void SceneHierarchyPanel::draw_sprite_renderer_component(Entity entity) {
	panel_utils::draw_component<component::SpriteRenderer>("Sprite Renderer", entity, [this](auto& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

		panel_utils::draw_button("Texture", {}, []() {});

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ContentBrowserPanel::DRAG_DROP_ID)) {
				const wchar_t* path                = reinterpret_cast<const wchar_t*>(payload->Data);
				std::filesystem::path texture_path = std::filesystem::path(ContentBrowserPanel::ASSETS_DIRECTORY) / path;
				if (texture_path.has_extension() && texture_path.extension() == ".png") {
					component.texture = Texture2D::create(texture_path.string());
				} else {
					EC_CORE_TRACE("Only \".png\" is currently supported as textures.");
				}
			}
			ImGui::EndDragDropTarget();
		}

		panel_utils::draw_drag_float(
		    {.label               = "Tiling Factor",
		     .drag_float_settings = {{.button_settings     = {.reset_value = 1.F},
		                              .drag_float_settings = {.value_speed = 0.1F, .value_min = 0.F, .value_max = 100.F}}}},
		    component.tiling_factor);
	});
}

void SceneHierarchyPanel::draw_circle_renderer_component(Entity entity) {
	panel_utils::draw_component<component::CircleRenderer>("Circle Renderer", entity, [this](auto& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

		float thickness = component.thickness.in(au::unos);
		float fade      = component.fade.in(au::unos);

		panel_utils::draw_drag_float(
		    {.label               = "Thickness",
		     .drag_float_settings = {{.button_settings     = {.reset_value = 1.F},
		                              .drag_float_settings = {.value_speed = 0.025F, .value_min = 0.F, .value_max = 1.F}}}},
		    thickness);

		panel_utils::draw_drag_float(
		    {.label               = "Fade",
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.F},
		                              .drag_float_settings = {.value_speed = 0.0025F, .value_min = 0.F, .value_max = 1.F}}}},
		    fade);

		component.thickness = au::unos(thickness);
		component.fade      = au::unos(fade);
	});
}

void SceneHierarchyPanel::draw_rigid_body_2d_component(Entity entity) {
	panel_utils::draw_component<component::RigidBody2D>("Rigid Body 2D", entity, [](auto& component) {
		auto current_body_type_string = component::RigidBody2D::to_string(component.type);

		if (ImGui::BeginCombo("Body Type", current_body_type_string.c_str())) {
			for (const auto* body_type_string : component::RigidBody2D::body_type_strings) {
				bool is_selected = current_body_type_string.c_str() == body_type_string;
				if (ImGui::Selectable(body_type_string, is_selected)) {
					current_body_type_string = body_type_string;
					component.type           = component::RigidBody2D::from_string(body_type_string);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::Checkbox("Fixed Rotation", &component.fixed_rotation);
	});
}

void SceneHierarchyPanel::draw_box_collider_2d_component(Entity entity) {
	panel_utils::draw_component<component::BoxCollider2D>("Box Collider 2D", entity, [this](auto& component) {
		panel_utils::draw_drag_float(
		    {.label             = "Offset",
		     .text_column_width = units::pixels(70.F),
		     .drag_float_settings =
		         {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED, .button_text = "X"},
		           .drag_float_settings = {.drag_float_tag = "##X", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN, .button_text = "Y"},
		           .drag_float_settings = {.drag_float_tag = "##Y", .value_speed = 0.1F}}}},
		    component.offset);

		panel_utils::draw_drag_float(
		    {.label               = "Size",
		     .text_column_width   = units::pixels(70.F),
		     .drag_float_settings = {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED,
		                                                      .reset_value   = 0.5F,
		                                                      .button_text   = "Width"},
		                              .drag_float_settings = {.drag_float_tag = "##Width", .value_speed = 0.1F}},
		                             {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN,
		                                                      .reset_value   = 0.5F,
		                                                      .button_text   = "Height"},
		                              .drag_float_settings = {.drag_float_tag = "##Height", .value_speed = 0.1F}}}},
		    component.size);

		float density               = component.density.in(units::densities);
		float friction              = component.friction.in(units::newtons);
		float restitution           = component.restitution.in(au::unos);
		float restitution_threshold = component.restitution_threshold.in(au::unos);

		panel_utils::draw_drag_float(
		    {.label               = "Density",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 1.F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    density);

		panel_utils::draw_drag_float(
		    {.label               = "Friction",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.5F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    friction);

		panel_utils::draw_drag_float(
		    {.label               = "Restitution",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    restitution);

		panel_utils::draw_drag_float(
		    {.label               = "Restitution Threshold",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.5F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F}}}},
		    restitution_threshold);

		component.density               = units::densities(density);
		component.friction              = units::newtons(friction);
		component.restitution           = au::unos(restitution);
		component.restitution_threshold = au::unos(restitution_threshold);
	});
}

void SceneHierarchyPanel::draw_circle_collider_2d_component(Entity entity) {
	panel_utils::draw_component<component::CircleCollider2D>("Circle Collider 2D", entity, [this](auto& component) {
		panel_utils::draw_drag_float(
		    {.label = "Offset",
		     .drag_float_settings =
		         {{.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_RED, .button_text = "X"},
		           .drag_float_settings = {.drag_float_tag = "##X", .value_speed = 0.1F}},
		          {.button_settings     = {.button_colors = panel_utils::BUTTON_COLORS_GREEN, .button_text = "Y"},
		           .drag_float_settings = {.drag_float_tag = "##Y", .value_speed = 0.1F}}}},
		    component.offset);

		float radius                = component.radius.in(au::meters);
		float density               = component.density.in(units::densities);
		float friction              = component.friction.in(units::newtons);
		float restitution           = component.restitution.in(au::unos);
		float restitution_threshold = component.restitution_threshold.in(au::unos);

		panel_utils::draw_drag_float(
		    {.label               = "Radius",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.5F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.0000001F}}}},
		    radius);

		panel_utils::draw_drag_float(
		    {.label               = "Density",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 1.F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    density);

		panel_utils::draw_drag_float(
		    {.label               = "Friction",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.5F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    friction);

		panel_utils::draw_drag_float(
		    {.label               = "Restitution",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F, .value_max = 1.F}}}},
		    restitution);

		panel_utils::draw_drag_float(
		    {.label               = "Restitution Threshold",
		     .text_column_width   = units::pixels(160.F),
		     .drag_float_settings = {{.button_settings     = {.reset_value = 0.5F},
		                              .drag_float_settings = {.value_speed = 0.01F, .value_min = 0.F}}}},
		    restitution_threshold);

		component.radius                = au::meters(radius);
		component.density               = units::densities(density);
		component.friction              = units::newtons(friction);
		component.restitution           = au::unos(restitution);
		component.restitution_threshold = au::unos(restitution_threshold);
	});
}

void SceneHierarchyPanel::draw_sub_texture_component(Entity entity) {
	panel_utils::draw_component<component::SubTexture>("Sub texture", entity, [&](auto& component) {
		auto id           = static_cast<int>(component.sub_texture->get_texture()->get_renderer_id());
		auto tile_index_x = component.sub_texture->get_index_x().in<int>(units::pixels);
		auto tile_index_y = component.sub_texture->get_index_y().in<int>(units::pixels);
		auto tile_width   = component.sub_texture->get_width().in<int>(units::pixels);
		auto tile_height  = component.sub_texture->get_height().in<int>(units::pixels);

		ImGui::Text(std::string("ID: " + std::to_string(id)).c_str());
		ImGui::InputInt("Index X", &tile_index_x, 1, 2);
		ImGui::InputInt("Index Y", &tile_index_y, 1, 2);
		ImGui::InputInt("Width", &tile_width, 1, 2);
		ImGui::InputInt("Height", &tile_height, 1, 2);

		component.sub_texture->set_index_x(units::pixels(tile_index_x));
		component.sub_texture->set_index_y(units::pixels(tile_index_y));
		component.sub_texture->set_tile_width(units::pixels(tile_width));
		component.sub_texture->set_tile_height(units::pixels(tile_height));
	});
}

}  // namespace eclipse