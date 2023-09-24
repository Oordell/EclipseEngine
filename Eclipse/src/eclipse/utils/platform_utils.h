#pragma once

#include "eclipse/common_types/file_path.h"

#include <string>
#include <optional>

namespace eclipse {

struct FileDialogResult {
	bool success {false};
	std::optional<FilePath> file_path = std::nullopt;
};

class FileDialogs {
public:
	static FileDialogResult open_file(const char* filter);
	static FileDialogResult save_file(const char* filter);
};

}  // namespace eclipse