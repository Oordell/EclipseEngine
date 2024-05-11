#include "scene_hierarchy_panel.h"
#include "content_browser_panel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <au.hh>

namespace eclipse {

SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) { set_context(context); }

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

template <typename ComponentType>
static void draw_component(const std::string& name, Entity entity, auto ui_function) {
	const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
	                                           ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding |
	                                           ImGuiTreeNodeFlags_AllowItemOverlap;
	if (entity.has_component<ComponentType>()) {
		auto& component                 = entity.get_component<ComponentType>();
		ImVec2 content_region_available = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4.0F, 4.0F});
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0F;
		ImGui::Separator();
		bool open =
		    ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(ComponentType).hash_code()), tree_node_flags, name.c_str());
		ImGui::PopStyleVar(2);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
		ImGui::SameLine(content_region_available.x - line_height * 0.5F);
		if (ImGui::Button("+", ImVec2 {line_height, line_height})) {
			ImGui::OpenPopup("ComponentSettings");
		}
		ImGui::PopStyleVar();

		bool remove_component = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove Component")) {
				remove_component = true;
			}
			ImGui::EndPopup();
		}

		if (open) {
			ui_function(component);
			ImGui::TreePop();
		}

		if (remove_component) {
			entity.remove_component<ComponentType>();
		}
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

	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("AddComponent");
	}
	if (ImGui::BeginPopup("AddComponent")) {
		add_pop_up_option<component::Camera>(entity, "Camera");
		add_pop_up_option<component::SpriteRenderer>(entity, "Sprite Renderer");
		add_pop_up_option<component::CircleRenderer>(entity, "Circle Renderer");
		add_pop_up_option<component::Color>(entity, "Color");
		add_pop_up_option<component::RigidBody2D>(entity, "Rigid Body 2D");
		add_pop_up_option<component::BoxCollider2D>(entity, "Box Collider 2D");
		add_pop_up_option<component::CircleCollider2D>(entity, "Circle Collider 2D");
		ImGui::EndPopup();
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar(2);
}

template <class Component>
void SceneHierarchyPanel::add_pop_up_option(Entity entity, const std::string& menu_item) {
	if (!entity.has_component<Component>() && ImGui::MenuItem(menu_item.c_str())) {
		selection_context_.add_component<Component>();
		ImGui::CloseCurrentPopup();
	}
}

void SceneHierarchyPanel::draw_transform_component(Entity entity) {
	draw_component<component::Transform>("Transform", entity, [this](auto& component) {
		draw_vec3_control({.values = component.translation, .label = "Translation"});
		glm::vec3 rotation = glm::degrees(component.rotation);
		draw_vec3_control({.values = rotation, .label = "Rotation"});
		component.rotation = glm::radians(rotation);
		draw_vec3_control({.values = component.scale, .label = "Scale", .reset_value = 1.0F});
	});
}

void SceneHierarchyPanel::draw_camera_component(Entity entity) {
	draw_component<component::Camera>("Camera", entity, [](auto& component) {
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
			float perspective_fov = camera.get_perspective_vertical_fov().in(au::degrees);
			if (ImGui::DragFloat("Vertical FOV", &perspective_fov)) {
				camera.set_perspective_vertical_fov(au::degrees(perspective_fov).as(au::radians));
			}

			float perspective_near_clip = camera.get_perspective_near_clip();
			if (ImGui::DragFloat("Near", &perspective_near_clip)) {
				camera.set_perspective_near_clip(perspective_near_clip);
			}

			float perspective_far_clip = camera.get_perspective_far_clip();
			if (ImGui::DragFloat("Far", &perspective_far_clip)) {
				camera.set_perspective_far_clip(perspective_far_clip);
			}
		}

		if (camera.get_projection_type() == ProjectionType::orthographic) {
			float ortho_size = camera.get_orthographic_size();
			if (ImGui::DragFloat("Size", &ortho_size)) {
				camera.set_orthographic_size(ortho_size);
			}

			float ortho_near_clip = camera.get_orthographic_near_clip();
			if (ImGui::DragFloat("Near", &ortho_near_clip)) {
				camera.set_orthographic_near_clip(ortho_near_clip);
			}

			float ortho_far_clip = camera.get_orthographic_far_clip();
			if (ImGui::DragFloat("Far", &ortho_far_clip)) {
				camera.set_orthographic_far_clip(ortho_far_clip);
			}
		}

		ImGui::Checkbox("Fixed Aspect Ratio", &component.fixed_aspect_ratio);
	});
}

void SceneHierarchyPanel::draw_color_component(Entity entity) {
	draw_component<component::Color>("Color", entity,
	                                 [](auto& component) { ImGui::ColorEdit4("Color", glm::value_ptr(component.color)); });
}

void SceneHierarchyPanel::draw_sprite_renderer_component(Entity entity) {
	draw_component<component::SpriteRenderer>("Sprite Renderer", entity, [](auto& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

		ImGui::Button("Texture", ImVec2(100.F, 0.F));

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

		ImGui::DragFloat("Tiling Factor", &component.tiling_factor, 0.1F, 0.F, 100.F);
	});
}

void SceneHierarchyPanel::draw_circle_renderer_component(Entity entity) {
	draw_component<component::CircleRenderer>("Circle Renderer", entity, [](auto& component) {
		ImGui::ColorEdit4("Color", glm::value_ptr(component.color));

		float thickness = component.thickness.in(au::unos);
		ImGui::DragFloat("Thickness", &thickness, 0.025F, 0.F, 1.F);
		component.thickness = au::unos(thickness);

		float fade = component.fade.in(au::unos);
		ImGui::DragFloat("Face", &fade, 0.00025F, 0.F, 1.F);
		component.fade = au::unos(fade);
	});
}

void SceneHierarchyPanel::draw_rigid_body_2d_component(Entity entity) {
	draw_component<component::RigidBody2D>("Rigid Body 2D", entity, [](auto& component) {
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
	draw_component<component::BoxCollider2D>("Box Collider 2D", entity, [](auto& component) {
		ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
		ImGui::DragFloat2("Size", glm::value_ptr(component.size));

		float density               = component.density.in(units::densities);
		float friction              = component.friction.in(units::newtons);
		float restitution           = component.restitution.in(au::unos);
		float restitution_threshold = component.restitution_threshold.in(au::unos);

		ImGui::DragFloat("Density", &density, .01F, .0F, 1.F);
		ImGui::DragFloat("Friction", &friction, .01F, .0F, 1.F);
		ImGui::DragFloat("Restitution", &restitution, .01F, .0F, 1.F);
		ImGui::DragFloat("Restitution Threshold", &restitution_threshold, .01F, .0F);

		component.density               = units::densities(density);
		component.friction              = units::newtons(friction);
		component.restitution           = au::unos(restitution);
		component.restitution_threshold = au::unos(restitution_threshold);
	});
}

void SceneHierarchyPanel::draw_circle_collider_2d_component(Entity entity) {
	draw_component<component::CircleCollider2D>("Circle Collider 2D", entity, [](auto& component) {
		ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));

		float radius                = component.radius.in(au::meters);
		float density               = component.density.in(units::densities);
		float friction              = component.friction.in(units::newtons);
		float restitution           = component.restitution.in(au::unos);
		float restitution_threshold = component.restitution_threshold.in(au::unos);

		ImGui::DragFloat("Radius", &radius);
		ImGui::DragFloat("Density", &density, .01F, .0F, 1.F);
		ImGui::DragFloat("Friction", &friction, .01F, .0F, 1.F);
		ImGui::DragFloat("Restitution", &restitution, .01F, .0F, 1.F);
		ImGui::DragFloat("Restitution Threshold", &restitution_threshold, .01F, .0F);

		component.radius                = au::meters(radius);
		component.density               = units::densities(density);
		component.friction              = units::newtons(friction);
		component.restitution           = au::unos(restitution);
		component.restitution_threshold = au::unos(restitution_threshold);
	});
}

void SceneHierarchyPanel::draw_vec3_control(const Vec3Controls& controls) {
	ImGuiIO& io                      = ImGui::GetIO();
	auto [font_was_found, font_data] = fonts::FontLibrary::get_font_index_by_name("Roboto-Bold");
	if (!font_was_found) {
		EC_FATAL("The font was not found!");
	}
	ImFont* bold_font = io.Fonts->Fonts[font_data.index];

	ImGui::PushID(controls.label.c_str());
	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, controls.column_width);
	ImGui::Text(controls.label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {4.0F, 2.0F});

	float line_height  = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0F;
	ImVec2 button_size = {line_height + 3.0F, line_height};

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8F, 0.1F, 0.15F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9F, 0.2F, 0.2F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8F, 0.1F, 0.15F, 1.0F});
	ImGui::PushFont(bold_font);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
	if (ImGui::Button("X", button_size)) {
		controls.values.x = controls.reset_value;
	}
	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &controls.values.x, 0.1F, 0.0F, 0.0F, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2F, 0.7F, 0.2F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3F, 0.8F, 0.3F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2F, 0.7F, 0.2F, 1.0F});
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
	ImGui::PushFont(bold_font);
	if (ImGui::Button("Y", button_size)) {
		controls.values.y = controls.reset_value;
	}
	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &controls.values.y, 0.1F, 0.0F, 0.0F, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1F, 0.25F, 0.8F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2F, 0.35F, 0.9F, 1.0F});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1F, 0.25F, 0.8F, 1.0F});
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
	ImGui::PushFont(bold_font);
	if (ImGui::Button("Z", button_size)) {
		controls.values.z = controls.reset_value;
	}
	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &controls.values.z, 0.1F, 0.0F, 0.0F, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

}  // namespace eclipse