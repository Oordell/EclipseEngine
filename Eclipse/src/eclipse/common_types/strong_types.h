#pragma once

#include "eclipse/core.h"
#include <string>

namespace eclipse {

class ECLIPSE_API FilePath {
public:
	explicit FilePath(const std::string& p) : value_(p) {}

	explicit operator std::string_view() const { return value_; }

	explicit operator std::string() const { return value_; }

	const std::string& value() const { return value_; }

private:
	std::string value_;
};

}  // namespace eclipse