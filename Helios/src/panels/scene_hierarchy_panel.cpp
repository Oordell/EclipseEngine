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
}

}  // namespace eclipse