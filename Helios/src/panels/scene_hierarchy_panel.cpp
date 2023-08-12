#include "scene_hierarchy_panel.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace eclipse {

SceneHierarchyPanel::SceneHierarchyPanel(const ref<Scene>& context) { set_context(context); }

void SceneHierarchyPanel::set_context(const ref<Scene>& context) { context_ = context; }

void SceneHierarchyPanel::on_imgui_render() {
	ImGui::Begin("Scene Hierarchy");

	context_->get_registry().each([&](auto entity_id) {
		Entity entity {entity_id, ref<Scene>(context_)};
		draw_entity_node(entity);
	});

	if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		selection_context_ = {};
	}

	ImGui::End();

	ImGui::Begin("Properties");
	if (selection_context_) {
		draw_components(selection_context_);
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
	if (opened) {
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::draw_components(Entity entity) {
	if (entity.has_component<component::Tag>()) {
		auto& tag = entity.get_component<component::Tag>().tag;

		static char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			tag = std::string(buffer);
		}
	}

	if (entity.has_component<component::Transform>()) {
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::Transform).hash_code()),
		                      ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
			auto& transform = entity.get_component<component::Transform>().transform;
			ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1F);
			ImGui::TreePop();
		}
	}

	if (entity.has_component<component::Camera>()) {
		if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(component::Camera).hash_code()), ImGuiTreeNodeFlags_DefaultOpen,
		                      "Camera")) {
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

}  // namespace eclipse