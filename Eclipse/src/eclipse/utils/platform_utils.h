#pragma once

#include "eclipse/common_types/file_path.h"

#include <string>
#include <optional>

namespace eclipse {

class FileDialogs {
public:
	static std::optional<FilePath> open_file(const char* filter);
	static std::optional<FilePath> save_file(const char* filter);
};

}  // namespace eclipse