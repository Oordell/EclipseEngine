#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <concepts>

#include "Eclipse.h"

namespace eclipse {

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept IsDragFloatable = IsAnyOf<T, float, glm::vec2, glm::vec3>;

class SceneHierarchyPanel {
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(const ref<Scene>& context);
	void set_context(const ref<Scene>& context);
	void on_imgui_render();
	void set_selected_entity(Entity entity);

	[[nodiscard]] Entity get_selected_entity() const { return selection_context_; }

private:
	void draw_entity_node(Entity entity);
	void draw_components(Entity entity);
	void draw_tag_component(Entity entity);
	void draw_transform_component(Entity entity);
	void draw_camera_component(Entity entity);
	void draw_color_component(Entity entity);
	void draw_sprite_renderer_component(Entity entity);
	void draw_circle_renderer_component(Entity entity);
	void draw_rigid_body_2d_component(Entity entity);
	void draw_box_collider_2d_component(Entity entity);
	void draw_circle_collider_2d_component(Entity entity);

	template <class Component>
	void add_pop_up_option(Entity entity, const std::string& menu_item);

	ImFont* get_font(const std::string& font_name);

	ImVec2 get_button_size(const std::string& button_text);

	struct ButtonColors {
		const ImVec4 standard = {0.431F, 0.427F, 0.875F, 1.F};
		const ImVec4 hovered  = {0.495F, 0.495F, 0.925F, 1.F};
		const ImVec4 active   = {0.431F, 0.427F, 0.875F, 1.F};
	};

	static constexpr ButtonColors BUTTON_COLORS_DEFAULT {.standard = {0.431F, 0.427F, 0.875F, 1.F},
	                                                     .hovered  = {0.495F, 0.495F, 0.925F, 1.F},
	                                                     .active   = {0.431F, 0.427F, 0.875F, 1.F}};
	static constexpr ButtonColors BUTTON_COLORS_RED {
	    .standard = {0.8F, 0.1F, 0.15F, 1.0F}, .hovered = {0.9F, 0.2F, 0.2F, 1.0F}, .active = {0.8F, 0.1F, 0.15F, 1.0F}};
	static constexpr ButtonColors BUTTON_COLORS_GREEN {
	    .standard = {0.2F, 0.7F, 0.2F, 1.0F}, .hovered = {0.3F, 0.8F, 0.3F, 1.0F}, .active = {0.2F, 0.7F, 0.2F, 1.0F}};
	static constexpr ButtonColors BUTTON_COLORS_BLUE {
	    .standard = {0.1F, 0.25F, 0.8F, 1.0F}, .hovered = {0.2F, 0.35F, 0.9F, 1.0F}, .active = {0.1F, 0.25F, 0.8F, 1.0F}};

	struct ButtonSettings {
		const ButtonColors& button_colors {BUTTON_COLORS_DEFAULT};
		const float reset_value {0.F};
		const std::string& button_text {"Reset"};
	};

	struct DragFloatSettings {
		const std::string& drag_float_tag {"##Reset"};
		float value_speed = 1.0F;
		float value_min   = 0.F;
		float value_max   = 0.F;
		const std::string& value_format {"%.2f"};
	};

	struct CombinedButtonAndDragFloatSettings {
		const ButtonSettings& button_settings;
		const DragFloatSettings& drag_float_settings;
	};

	struct DragFloatsMetaData {
		const std::string& label                       = "";
		au::QuantityF<units::Pixels> text_column_width = units::pixels(100.0F);
		std::vector<CombinedButtonAndDragFloatSettings> drag_float_settings;
	};

	struct ButtonAndDragFloatSettings {
		const CombinedButtonAndDragFloatSettings& button_and_float_settings;
		const ImVec2& button_size;
		ImFont* font;
	};

	template <IsDragFloatable Type>
	void draw_drag_float(const DragFloatsMetaData& settings, Type& values) {
		auto* bold_font = get_font("Roboto-Bold");

		ImGui::PushID(settings.label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, settings.text_column_width.in(units::pixels));
		ImGui::Text(settings.label.c_str());
		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {4.0F, 2.0F});

		uint32_t num_of_values = 0;

		if constexpr (std::same_as<Type, float>) {
			EC_ASSERT(settings.drag_float_settings.size() == 1, "Wrong size for settings!");
			num_of_values = 1;
		} else if constexpr (std::same_as<Type, glm::vec2>) {
			EC_ASSERT(settings.drag_float_settings.size() == 2, "Wrong size for settings!");
			num_of_values = 2;
		} else if constexpr (std::same_as<Type, glm::vec3>) {
			EC_ASSERT(settings.drag_float_settings.size() == 3, "Wrong size for settings!");
			num_of_values = 3;
		} else {
			EC_FATAL("Couldn't determine the type of the input...");
		}

		EC_ASSERT(num_of_values > 0, "There must be more that one value!");

		for (uint32_t i = 0; i < num_of_values; i++) {
			if (i == 0) {
				// Only first loop
				ImGui::PushMultiItemsWidths(num_of_values, ImGui::CalcItemWidth());
			} else {
				// Not first, but between each loop, and not after the last
				ImGui::PopItemWidth();
				ImGui::SameLine();
			}

			ImVec2 button_size = get_button_size(settings.drag_float_settings[i].button_settings.button_text);

			if constexpr (std::same_as<Type, float>) {
				draw_labeled_drag_float(
				    {.button_and_float_settings = {.button_settings     = settings.drag_float_settings[i].button_settings,
				                                   .drag_float_settings = settings.drag_float_settings[i].drag_float_settings},
				     .button_size               = button_size,
				     .font                      = bold_font},
				    values);
			} else if constexpr (std::same_as<Type, glm::vec2> || std::same_as<Type, glm::vec3>) {
				draw_labeled_drag_float(
				    {.button_and_float_settings = {.button_settings     = settings.drag_float_settings[i].button_settings,
				                                   .drag_float_settings = settings.drag_float_settings[i].drag_float_settings},
				     .button_size               = button_size,
				     .font                      = bold_font},
				    values[i]);
			}
		}

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void draw_labeled_drag_float(const ButtonAndDragFloatSettings& settings, float& value);

	ref<Scene> context_;
	Entity selection_context_;
};

}  // namespace eclipse