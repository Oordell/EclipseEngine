#pragma once

#include "eclipse/core/core.h"
#include "eclipse/common_types/file_path.h"

#include <vector>

namespace eclipse::fonts {

struct FontAdderResult {
	uint32_t index_of_new_font {};
	bool success {false};
};

struct FontData {
	uint16_t index {};
	std::string_view name {};
	FilePath file_path {"Nan"};
};

struct FontGetterResult {
	bool was_found {false};
	FontData data {};
};

class FontLibrary {
public:
	FontLibrary(const FontLibrary&)            = delete;
	FontLibrary(FontLibrary&&)                 = delete;
	FontLibrary& operator=(const FontLibrary&) = delete;
	FontLibrary& operator=(FontLibrary&&)      = delete;
	~FontLibrary()                             = default;

	static FontLibrary& get() { return instance_; }

	static FontAdderResult add_font(const FilePath& path, std::string_view name, float size_pixels = 18.0F);
	static FontGetterResult get_font_index_by_name(std::string_view name);

private:
	FontLibrary() = default;
	FontAdderResult add_font_impl(const FilePath& path, std::string_view name, float size_pixels);
	FontGetterResult get_font_index_by_name_impl(std::string_view name);

	std::vector<FontData> fonts_added_ = {};
	static FontLibrary instance_;
};

}  // namespace eclipse::fonts