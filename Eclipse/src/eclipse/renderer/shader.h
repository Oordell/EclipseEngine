#pragma once

#include "eclipse/core.h"

#include <string>

namespace eclipse {

class ECLIPSE_API Shader {
public:
	virtual ~Shader() = default;

	virtual void bind() const   = 0;
	virtual void unbind() const = 0;

	static Shader* create(const std::string& vertex_src, const std::string& fragment_src);
};

}  // namespace eclipse