#include "ecpch.h"
#include "content_browser_panel.h"

#include <imgui/imgui.h>

namespace eclipse {

void ContentBrowserPanel::on_imgui_render() {
	ImGui::Begin("Content Browser");

	if (current_directory_ != assets_path_) {
		if (ImGui::Button("<-")) {
			current_directory_ = current_directory_.parent_path();
		}
	}

	static float padding        = 12.5F;
	static float thumbnail_size = 64.0F;
	float cell_size             = padding + thumbnail_size;
	float panel_width           = ImGui::GetContentRegionAvail().x;
	int column_count            = std::max(1, static_cast<int>(panel_width / cell_size));

	ImGui::Columns(column_count, 0, false);

	render_directory_content(thumbnail_size);

	ImGui::Columns(1);
	ImGui::SliderFloat("Thumbnail size", &thumbnail_size, 16.F, 512.F);
	ImGui::SliderFloat("Padding", &padding, 0.F, 32.F);

	ImGui::End();
}

void ContentBrowserPanel::render_directory_content(float thumbnail_size) {
	for (auto& directory_entry : std::filesystem::directory_iterator(current_directory_)) {
		const auto& path            = directory_entry.path();
		auto relative_path          = std::filesystem::relative(path, assets_path_);
		std::string filename_string = relative_path.filename().string();
		ImGui::PushID(filename_string.c_str());

		ref<Texture2D> icon = directory_entry.is_directory() ? directory_icon_ : file_icon_;
		auto id             = static_cast<uint64_t>(icon->get_renderer_id());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.F, 0.F, 0.F, 0.F));  // Add transparent background
		ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), {thumbnail_size, thumbnail_size}, {0, 1}, {1, 0});

		if (ImGui::BeginDragDropSource()) {
			const wchar_t* item_path = relative_path.c_str();
			ImGui::SetDragDropPayload(DRAG_DROP_ID, item_path, (wcslen(item_path) + 1) * sizeof(wchar_t), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (directory_entry.is_directory()) {
				current_directory_ /= path.filename();
			}
		}
		ImGui::TextWrapped(filename_string.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}
}

}  // namespace eclipse