#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/glm.hpp>
#include "Eclipse.h"
#include <concepts>
#include <string>

namespace eclipse::panel_utils {

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

template <typename T>
concept IsDragFloatable = IsAnyOf<T, float, glm::vec2, glm::vec3>;

template <typename T>
concept IsDragInteger = IsAnyOf<T, int, int[2], int[3]>;

struct ButtonColors {
	const ImVec4 standard = {0.431F, 0.427F, 0.875F, 1.F};
	const ImVec4 hovered  = {0.495F, 0.495F, 0.925F, 1.F};
	const ImVec4 active   = {0.431F, 0.427F, 0.875F, 1.F};
};

static inline constexpr ButtonColors BUTTON_COLORS_DEFAULT {.standard = {0.431F, 0.427F, 0.875F, 1.F},
                                                            .hovered  = {0.495F, 0.495F, 0.925F, 1.F},
                                                            .active   = {0.431F, 0.427F, 0.875F, 1.F}};
static inline constexpr ButtonColors BUTTON_COLORS_RED {
    .standard = {0.8F, 0.1F, 0.15F, 1.0F}, .hovered = {0.9F, 0.2F, 0.2F, 1.0F}, .active = {0.8F, 0.1F, 0.15F, 1.0F}};
static inline constexpr ButtonColors BUTTON_COLORS_GREEN {
    .standard = {0.2F, 0.7F, 0.2F, 1.0F}, .hovered = {0.3F, 0.8F, 0.3F, 1.0F}, .active = {0.2F, 0.7F, 0.2F, 1.0F}};
static inline constexpr ButtonColors BUTTON_COLORS_BLUE {
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

struct DragIntegerSettings {
	const std::string& drag_integer_tag {"##Reset"};
	float value_speed = 1.F;
	int value_min     = 0;
	int value_max     = 0;
	const std::string& value_format {"%d"};
};

struct CombinedButtonAndDragIntegerSettings {
	const ButtonSettings& button_settings;
	const DragIntegerSettings& drag_integer_settings;
};

struct DragIntegerMetaData {
	const std::string& label                       = "";
	au::QuantityF<units::Pixels> text_column_width = units::pixels(100.0F);
	std::vector<CombinedButtonAndDragIntegerSettings> drag_integer_settings;
};

inline ImFont* get_font(const std::string& font_name) {
	ImGuiIO& io                      = ImGui::GetIO();
	auto [font_was_found, font_data] = fonts::FontLibrary::get_font_index_by_name("Roboto-Bold");
	if (!font_was_found) {
		EC_FATAL("The font was not found!");
	}
	return io.Fonts->Fonts[font_data.index];
}

inline ImVec2 get_button_size(const std::string& button_text) {
	float line_height              = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0F;
	auto [text_width, text_height] = ImGui::CalcTextSize(button_text.c_str());
	return {line_height + text_width - 7.F, line_height};
}

template <std::invocable Func>
inline constexpr void draw_button(const std::string& button_text, const ButtonColors& colors,
                                  const Func& on_press_callback) {
	auto* bold_font    = get_font("Roboto-Bold");
	ImVec2 button_size = get_button_size(button_text);

	ImGui::PushStyleColor(ImGuiCol_Button, colors.standard);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors.hovered);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors.active);
	ImGui::PushFont(bold_font);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.F);

	if (ImGui::Button(button_text.c_str(), button_size)) {
		on_press_callback();
	}

	ImGui::PopFont();
	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(3);
}

inline void draw_labeled_drag_float(const CombinedButtonAndDragFloatSettings& settings, float& value) {
	draw_button(settings.button_settings.button_text, settings.button_settings.button_colors,
	            [&]() { value = settings.button_settings.reset_value; });

	ImGui::SameLine();
	ImGui::DragFloat(settings.drag_float_settings.drag_float_tag.c_str(), &value, settings.drag_float_settings.value_speed,
	                 settings.drag_float_settings.value_min, settings.drag_float_settings.value_max,
	                 settings.drag_float_settings.value_format.c_str());
}

inline void draw_labeled_drag_integer(const CombinedButtonAndDragIntegerSettings& settings, int& value) {
	draw_button(settings.button_settings.button_text, settings.button_settings.button_colors,
	            [&]() { value = static_cast<int>(settings.button_settings.reset_value); });

	ImGui::SameLine();

	ImGui::DragInt(settings.drag_integer_settings.drag_integer_tag.c_str(), &value,
	               settings.drag_integer_settings.value_speed, settings.drag_integer_settings.value_min,
	               settings.drag_integer_settings.value_max, settings.drag_integer_settings.value_format.c_str(),
	               ImGuiSliderFlags_AlwaysClamp);
}

template <component::IsComponent ComponentType>
constexpr void draw_component(const std::string& name, Entity entity, auto ui_function) {
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

		draw_button("+", {}, []() { ImGui::OpenPopup("ComponentSettings"); });
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

template <IsDragFloatable Type>
inline constexpr void draw_drag_float(const DragFloatsMetaData& settings, Type& values) {
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

	EC_ASSERT(num_of_values > 0 && num_of_values < 4, "Value count must be between 1 and 3!");

	for (uint32_t i = 0; i < num_of_values; i++) {
		if (i == 0) {  // Only first loop
			ImGui::PushMultiItemsWidths(num_of_values, ImGui::CalcItemWidth());
		} else {  // Not first, but between each loop, and not after the last
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		if constexpr (std::same_as<Type, float>) {
			draw_labeled_drag_float({.button_settings     = settings.drag_float_settings[i].button_settings,
			                         .drag_float_settings = settings.drag_float_settings[i].drag_float_settings},
			                        values);
		} else if constexpr (std::same_as<Type, glm::vec2> || std::same_as<Type, glm::vec3>) {
			draw_labeled_drag_float({.button_settings     = settings.drag_float_settings[i].button_settings,
			                         .drag_float_settings = settings.drag_float_settings[i].drag_float_settings},
			                        values[i]);
		}
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

template <IsDragInteger Type>
inline constexpr void draw_drag_integer(const DragIntegerMetaData& settings, Type& values) {
	ImGui::PushID(settings.label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, settings.text_column_width.in(units::pixels));
	ImGui::Text(settings.label.c_str());
	ImGui::NextColumn();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {4.0F, 2.0F});

	uint32_t num_of_values = 0;

	if constexpr (std::same_as<Type, int>) {
		EC_ASSERT(settings.drag_integer_settings.size() == 1, "Wrong size for settings!");
		num_of_values = 1;
	} else if constexpr (std::same_as<Type, int[2]>) {
		EC_ASSERT(settings.drag_integer_settings.size() == 2, "Wrong size for settings!");
		num_of_values = 2;
	} else if constexpr (std::same_as<Type, int[3]>) {
		EC_ASSERT(settings.drag_integer_settings.size() == 3, "Wrong size for settings!");
		num_of_values = 3;
	} else {
		EC_FATAL("Couldn't determine the type of the input...");
	}

	EC_ASSERT(num_of_values > 0 && num_of_values < 4, "Value count must be between 1 and 3!");

	for (uint32_t i = 0; i < num_of_values; i++) {
		if (i == 0) {  // Only first loop
			ImGui::PushMultiItemsWidths(num_of_values, ImGui::CalcItemWidth());
		} else {  // Not first, but between each loop, and not after the last
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		if constexpr (std::same_as<Type, int>) {
			draw_labeled_drag_integer({.button_settings       = settings.drag_integer_settings[i].button_settings,
			                           .drag_integer_settings = settings.drag_integer_settings[i].drag_integer_settings},
			                          values);
		} else if constexpr (std::same_as<Type, int[2]> || std::same_as<Type, int[3]>) {
			draw_labeled_drag_integer({.button_settings       = settings.drag_integer_settings[i].button_settings,
			                           .drag_integer_settings = settings.drag_integer_settings[i].drag_integer_settings},
			                          values[i]);
		}
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

}  // namespace eclipse::panel_utils