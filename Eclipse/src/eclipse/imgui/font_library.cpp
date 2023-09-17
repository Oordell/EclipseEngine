#include "ecpch.h"
#include "font_library.h"

#include <imgui.h>

namespace eclipse::fonts {

FontLibrary FontLibrary::instance_ = FontLibrary();

FontAdderResult FontLibrary::add_font(const FilePath& path, std::string_view name, float size_pixels) {
	return get().add_font_impl(path, name, size_pixels);
}

FontGetterResult FontLibrary::get_font_index_by_name(std::string_view name) {
	return get().get_font_index_by_name_impl(name);
}

FontAdderResult FontLibrary::add_font_impl(const FilePath& path, std::string_view name, float size_pixels) {
	ImGui::CreateContext();
	ImGuiIO& io               = ImGui::GetIO();
	ImFont* font              = io.Fonts->AddFontFromFileTTF(path.value().c_str(), size_pixels);
	auto current_num_of_fonts = static_cast<uint32_t>(fonts_added_.size());
	fonts_added_.emplace_back(current_num_of_fonts, name, path);
	return {current_num_of_fonts, true};
}

FontGetterResult FontLibrary::get_font_index_by_name_impl(std::string_view name) {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	for (size_t i = 0; i < fonts_added_.size(); i++) {
		if (fonts_added_[i].name == name) {
			return {true, fonts_added_[i]};
		}
	}
	return {false, {}};
}

}  // namespace eclipse::fonts