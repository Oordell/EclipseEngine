#pragma once

#include "eclipse/renderer/texture.h"

#include <filesystem>

namespace eclipse {

class ContentBrowserPanel {
public:
	void on_imgui_render();

	static constexpr const char* DRAG_DROP_ID               = "CONTENT_BROWSER_ITEM";
	static constexpr const char* DRAG_DROP_TEXTURE_SHEET_ID = "TEXTURE_SHEET_ITEM_";
	static constexpr const char* ASSETS_DIRECTORY           = "assets";

private:
	void render_directory_content(float thumbnail_size);

	const std::filesystem::path assets_path_ = ASSETS_DIRECTORY;
	std::filesystem::path current_directory_ = ASSETS_DIRECTORY;
	ref<Texture2D> directory_icon_           = Texture2D::create("resources/icons/content_browser/directory_icon.png");
	ref<Texture2D> file_icon_                = Texture2D::create("resources/icons/content_browser/file_icon.png");
};

}  // namespace eclipse