#pragma once

#include <filesystem>

namespace eclipse {

class ContentBrowserPanel {
public:
	ContentBrowserPanel() = default;
	void on_imgui_render();

private:
	static constexpr const char* ASSETS_DIRECTORY = "assets";
	const std::filesystem::path assets_path_      = ASSETS_DIRECTORY;
	std::filesystem::path current_directory_      = ASSETS_DIRECTORY;
};

}  // namespace eclipse