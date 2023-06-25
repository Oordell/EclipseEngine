// Basic texture shader

#type vertex
#version 330 core
			
layout(location = 0) in vec3 position_;
layout(location = 1) in vec2 tex_coord_;

uniform mat4 view_projection;
uniform mat4 transform;

out vec2 v_tex_coord_;

void main() {
	v_tex_coord_ = tex_coord_;
	gl_Position = view_projection * transform * vec4(position_, 1.0);
}

#type fragment
#version 330 core
			
layout(location = 0) out vec4 color;

in vec2 v_tex_coord_;

uniform sampler2D u_texture;
			
void main() {
	color = texture(u_texture, v_tex_coord_ * 10.0);
}