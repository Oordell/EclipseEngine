#include "scene_hierarchy_panel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) { set_context(context); }

void SceneHierarchyPanel::set_context(const ref<Scene>& context) { context_ = context; }

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

		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				selection_context_.add_component<component::Camera>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Sprite Renderer")) {
				selection_context_.add_component<component::SpriteRenderer>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Color")) {
				selection_context_.add_component<component::Color>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void SceneHierarchyPanel::draw_entity_node(Entity entity) {
	auto& tag_component = entity.get_component<component::Tag>();
	uint64_t entity_id  = static_cast<uint64_t>(entity);

	ImGuiTreeNodeFlags flags =
	    ((selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
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
}

void SceneHierarchyPanel::draw_tag_component(Entity entity) {
	if (entity.has_component<component::Tag>()) {
		auto& tag = entity.get_component<component::Tag>().tag;

		static char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			tag = std::string(buffer);
		}
	}
}

void SceneHierarchyPanel::draw_transform_component(Entity entity) {
	static const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
	if (entity.has_component<component::Transform>()) {
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::Transform).hash_code()), tree_node_flags,
		                      "Transform")) {
			auto& transform = entity.get_component<component::Transform>();
			draw_vec3_control({.values = transform.translation, .label = "Translation"});
			glm::vec3 rotation = glm::degrees(transform.rotation);
			draw_vec3_control({.values = rotation, .label = "Rotation"});
			transform.rotation = glm::radians(rotation);
			draw_vec3_control({.values = transform.scale, .label = "Scale", .reset_value = 1.0F});
			ImGui::TreePop();
		}
	}
}

void SceneHierarchyPanel::draw_camera_component(Entity entity) {
	static const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
	if (entity.has_component<component::Camera>()) {
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::Camera).hash_code()), tree_node_flags, "Camera")) {
			auto& camera_component = entity.get_component<component::Camera>();
			auto& camera           = camera_component.camera;

			ImGui::Checkbox("Primary", &camera_component.primary);

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
				float perspective_fov = utils::rad_to_deg(camera.get_perspective_vertical_fov());
				if (ImGui::DragFloat("Vertical FOV", &perspective_fov)) {
					camera.set_perspective_vertical_fov(utils::deg_to_rad(perspective_fov));
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

			ImGui::Checkbox("Fixed Aspect Ratio", &camera_component.fixed_aspect_ratio);

			ImGui::TreePop();
		}
	}
}

void SceneHierarchyPanel::draw_color_component(Entity entity) {
	static const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
	if (entity.has_component<component::Color>()) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4.0F, 4.0F});
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
		bool open =
		    ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::Color).hash_code()), tree_node_flags, "Color");
		ImGui::SameLine(ImGui::GetWindowWidth() - 25.0F);
		if (ImGui::Button("+", ImVec2 {20.0F, 20.0F})) {
			ImGui::OpenPopup("ComponentSettings");
		}
		ImGui::PopStyleVar(2);

		bool remove_component = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove Component")) {
				remove_component = true;
			}
			ImGui::EndPopup();
		}

		if (open) {
			auto& color = entity.get_component<component::Color>().color;
			ImGui::ColorEdit4("Color", glm::value_ptr(color));
			ImGui::TreePop();
		}

		if (remove_component) {
			entity.remove_component<component::Color>();
		}
	}
}

void SceneHierarchyPanel::draw_sprite_renderer_component(Entity entity) {
	static const ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | tree_node_flags;
	if (entity.has_component<component::SpriteRenderer>()) {
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::SpriteRenderer).hash_code()),
		                      ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer")) {
			auto& color = entity.get_component<component::SpriteRenderer>().color;
			ImGui::ColorEdit4("Color", glm::value_ptr(color));
			ImGui::TreePop();
		}
	}
}

void SceneHierarchyPanel::draw_vec3_control(const Vec3Controls& controls) {
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
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0F);
	if (ImGui::Button("X", button_size)) {
		controls.values.x = controls.reset_value;
	}
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
	if (ImGui::Button("Y", button_size)) {
		controls.values.y = controls.reset_value;
	}
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
	if (ImGui::Button("Z", button_size)) {
		controls.values.z = controls.reset_value;
	}
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