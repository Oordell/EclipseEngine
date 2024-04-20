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

	for (auto& directory_entry : std::filesystem::directory_iterator(current_directory_)) {
		const auto& path            = directory_entry.path();
		auto relative_path          = std::filesystem::relative(path, assets_path_);
		std::string filename_string = relative_path.filename().string();

		if (ImGui::Button(filename_string.c_str())) {
			if (directory_entry.is_directory()) {
				current_directory_ /= path.filename();
			}
		}
	}

	ImGui::End();
}

}  // namespace eclipse