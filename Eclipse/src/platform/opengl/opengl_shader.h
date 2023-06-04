#pragma once

#include "eclipse/renderer/shader.h"
#include <glm/glm.hpp>

namespace eclipse {

class ECLIPSE_API OpenGLShader : public Shader {
public:
	OpenGLShader(const std::string& vertex_src, const std::string& fragment_src);
	virtual ~OpenGLShader();

	void bind() const override;
	void unbind() const override;

	void upload_uniform_int(const std::string& name, int value);
	void upload_uniform_float(const std::string& name, float value);
	void upload_uniform_float2(const std::string& name, const glm::vec2& values);
	void upload_uniform_float3(const std::string& name, const glm::vec3& values);
	void upload_uniform_float4(const std::string& name, const glm::vec4& values);
	void upload_uniform_mat3(const std::string& name, const glm::mat4& matrix);
	void upload_uniform_mat4(const std::string& name, const glm::mat4& matrix);

private:
	uint32_t renderer_id_ = 0;
};

}  // namespace eclipse