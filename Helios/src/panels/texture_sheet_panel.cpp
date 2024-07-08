#include "ecpch.h"
#include "texture_sheet_panel.h"
#include "panel_utils.h"
#include "content_browser_panel.h"
#include "eclipse/scene/texture_sheets_components.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace eclipse {

TextureSheetPanel::TextureSheetPanel(const ref<Scene>& context) { set_context(context); }

void TextureSheetPanel::set_context(const ref<Scene>& context) { context_ = context; }

void TextureSheetPanel::on_imgui_render() {
	ImGui::Begin("Texture Sheets");

	draw_add_texture_sheet_button();
	draw_texture_sheet_list();

	ImGui::End();
}

void TextureSheetPanel::draw_add_texture_sheet_button() {
	static const auto default_sheet_thumbnail = Texture2D::create("resources/icons/content_browser/file_icon.png");
	static constexpr float thumbnail_size     = 80.0F;
	auto id                                   = static_cast<uint64_t>(default_sheet_thumbnail->get_renderer_id());
	ImGui::PushID(ContentBrowserPanel::DRAG_DROP_TEXTURE_SHEET_ID);
	if (ImGui::ImageButton(ContentBrowserPanel::DRAG_DROP_TEXTURE_SHEET_ID, reinterpret_cast<ImTextureID>(id),
	                       {thumbnail_size, thumbnail_size}, {0, 1}, {1, 0})) {
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ContentBrowserPanel::DRAG_DROP_ID)) {
			const wchar_t* path                = reinterpret_cast<const wchar_t*>(payload->Data);
			std::filesystem::path texture_path = std::filesystem::path(ContentBrowserPanel::ASSETS_DIRECTORY) / path;
			if (texture_path.has_extension() && texture_path.extension() == ".png") {
				auto texture_sheet = Texture2D::create(texture_path.string());
				component::TextureSheetComponent tsc;
				tsc.texture_sheet = make_ref<TextureSheet>(TextureSheetProperties {.texture            = texture_sheet,
				                                                                   .sub_tile_width     = units::pixels(16),
				                                                                   .sub_tile_height    = units::pixels(16),
				                                                                   .sub_tile_spacing_x = units::pixels(0),
				                                                                   .sub_tile_spacing_y = units::pixels(0)});
				auto entity       = context_->create_entity(texture_path.filename().string());
				entity.add_component<component::TextureSheetComponent>(tsc);
			} else {
				EC_CORE_TRACE("Only \".png\" is currently supported as textures.");
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopID();
}

void TextureSheetPanel::draw_texture_sheet_list() {
	Entity entity_to_delete;
	context_->get_registry().each([&](auto entity_id) {
		Entity entity {entity_id, context_.get()};

		if (entity.has_component<component::TextureSheetComponent>()) {
			auto& tsc                         = entity.get_component<component::TextureSheetComponent>();
			std::string entity_uuid           = std::to_string(entity.get_uuid().value());
			ref<Texture2D> preview_image      = tsc.texture_sheet->get_texture();
			static constexpr float image_size = 80.0F;
			auto preview_id                   = static_cast<uint64_t>(preview_image->get_renderer_id());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.F, 0.F, 0.F, 0.F));  // Add transparent background
			ImGui::Image(reinterpret_cast<ImTextureID>(preview_id), {image_size, image_size}, {0, 1}, {1, 0});

			ImGui::SameLine();
			static const auto trashcan_icon = Texture2D::create("resources/icons/trashcan.png");
			auto trashcan_id                = static_cast<uint64_t>(trashcan_icon->get_renderer_id());
			bool remove_texture_sheet       = false;
			if (ImGui::ImageButton(std::string(entity_uuid + "trash").c_str(), reinterpret_cast<ImTextureID>(trashcan_id),
			                       {image_size, image_size}, {0, 1}, {1, 0})) {
				remove_texture_sheet = true;
			}
			ImGui::PopStyleColor();

			if (remove_texture_sheet) {
				entity_to_delete = entity;
				return;
			}

			ImGui::SameLine();
			static const auto plus_icon = Texture2D::create("resources/icons/plus.png");
			auto plus_id                = static_cast<uint64_t>(plus_icon->get_renderer_id());
			bool plus_icon_pressed      = false;
			if (ImGui::ImageButton(std::string(entity_uuid + "plus").c_str(), reinterpret_cast<ImTextureID>(plus_id),
			                       {image_size, image_size}, {0, 1}, {1, 0})) {
				plus_icon_pressed = true;
			}
			if (plus_icon_pressed) {
				auto new_entity = context_->create_entity("New sub-texture entity");
				component::SubTexture st;
				st.sub_texture = make_ref<SubTexture2D>(SubTexture2DProperties {.texture_sheet = tsc.texture_sheet,
				                                                                .tile_index_x  = units::pixels(0),
				                                                                .tile_index_y  = units::pixels(0),
				                                                                .tile_width    = units::pixels(1),
				                                                                .tile_height   = units::pixels(1)});
				new_entity.add_component<component::SubTexture>(st);
			}

			ImGui::SameLine();

			ImGui::Text(entity_uuid.c_str());

			int sub_tile_size[2]    = {tsc.texture_sheet->get_tile_width_in_pixels().in<int>(units::pixels),
			                           tsc.texture_sheet->get_tile_height_in_pixels().in<int>(units::pixels)};
			int sub_tile_spacing[2] = {tsc.texture_sheet->get_tile_spacing_x_in_pixels().in<int>(units::pixels),
			                           tsc.texture_sheet->get_tile_spacing_y_in_pixels().in<int>(units::pixels)};

			std::string tag_x = "##X" + entity_uuid;
			std::string tag_y = "##Y" + entity_uuid;

			panel_utils::draw_drag_integer(
			    {.label                 = "Size",
			     .text_column_width     = units::pixels(80),
			     .drag_integer_settings = {{.button_settings       = {.button_colors = panel_utils::BUTTON_COLORS_RED,
			                                                          .reset_value   = 1.0F,
			                                                          .button_text   = "X"},
			                                .drag_integer_settings = {.drag_integer_tag = tag_x, .value_max = 50}},
			                               {.button_settings       = {.button_colors = panel_utils::BUTTON_COLORS_GREEN,
			                                                          .reset_value   = 1.0F,
			                                                          .button_text   = "Y"},
			                                .drag_integer_settings = {.drag_integer_tag = tag_y, .value_max = 50}}}},
			    sub_tile_size);

			panel_utils::draw_drag_integer(
			    {.label                 = "Spacing",
			     .text_column_width     = units::pixels(80),
			     .drag_integer_settings = {{.button_settings       = {.button_colors = panel_utils::BUTTON_COLORS_RED,
			                                                          .reset_value   = 0.F,
			                                                          .button_text   = "X"},
			                                .drag_integer_settings = {.drag_integer_tag = tag_x, .value_max = 10}},
			                               {.button_settings       = {.button_colors = panel_utils::BUTTON_COLORS_GREEN,
			                                                          .reset_value   = 0.F,
			                                                          .button_text   = "Y"},
			                                .drag_integer_settings = {.drag_integer_tag = tag_y, .value_max = 10}}}},
			    sub_tile_spacing);

			tsc.texture_sheet->set_tile_width(units::pixels(sub_tile_size[0]));
			tsc.texture_sheet->set_tile_height(units::pixels(sub_tile_size[1]));
			tsc.texture_sheet->set_tile_spacing_x(units::pixels(sub_tile_spacing[0]));
			tsc.texture_sheet->set_tile_spacing_y(units::pixels(sub_tile_spacing[1]));
		}
	});

	if (entity_to_delete) {
		destroy_entities_that_uses_sheet(entity_to_delete);
	}
}

void TextureSheetPanel::destroy_entities_that_uses_sheet(Entity texture_sheet_entity) {
	auto texture_renderer_id = texture_sheet_entity.get_component<component::TextureSheetComponent>()
	                               .texture_sheet->get_texture()
	                               ->get_renderer_id();
	auto sub_texture_view = context_->get_registry().view<component::SubTexture>();
	for (auto entity : sub_texture_view) {
		const auto& sub_texture_component = sub_texture_view.get<component::SubTexture>(entity);
		auto sub_texture_renderer_id      = sub_texture_component.sub_texture->get_texture()->get_renderer_id();
		if (sub_texture_renderer_id == texture_renderer_id) {
			auto e = Entity {entity, context_.get()};
			notify_subscribers(e);
			context_->destroy_entity(e);
		}
	}
	context_->destroy_entity(texture_sheet_entity);
}

}  // namespace eclipse