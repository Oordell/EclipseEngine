#pragma once

#include <string>
#include <optional>
#include <filesystem>

namespace eclipse {

class FileDialogs {
public:
	static std::optional<std::filesystem::path> open_file(const char* filter);
	static std::optional<std::filesystem::path> save_file(const char* filter);
};

}  // namespace eclipse