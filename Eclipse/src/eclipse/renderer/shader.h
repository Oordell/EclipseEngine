#pragma once

#include <string>
#include "eclipse/core.h"

namespace eclipse {

class ECLIPSE_API Shader {
public:
	Shader(const std::string& vertex_src, const std::string& fragment_src);
	~Shader();

	void bind() const;
	void unbind() const;

private:
	uint32_t renderer_id_ = 0;
};

}  // namespace eclipse