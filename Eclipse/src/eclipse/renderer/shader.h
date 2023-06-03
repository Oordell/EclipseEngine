#pragma once

#include "eclipse/core.h"

#include <string>
#include <glm/glm.hpp>

namespace eclipse {

class ECLIPSE_API Shader {
public:
	Shader(const std::string& vertex_src, const std::string& fragment_src);
	~Shader();

	void bind() const;
	void unbind() const;

	void upload_uniform_mat4(const std::string& name, const glm::mat4& matrix);

private:
	uint32_t renderer_id_ = 0;
};

}  // namespace eclipse