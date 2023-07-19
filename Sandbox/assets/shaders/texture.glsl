// Basic texture shader

#type vertex
#version 330 core
			
layout(location = 0) in vec3 position_;
layout(location = 1) in vec4 color_;
layout(location = 2) in vec2 tex_coord_;

uniform mat4 view_projection;

out vec2 v_tex_coord_;
out vec4 v_color;

void main() {
	v_tex_coord_ = tex_coord_;
	v_color = color_;
	gl_Position = view_projection * vec4(position_, 1.0);
}

#type fragment
#version 330 core
			
layout(location = 0) out vec4 color;

in vec2 v_tex_coord_;
in vec4 v_color;

uniform vec4 u_color;
uniform float u_tiling_factor;
uniform sampler2D u_texture;
			
void main() {
	// color = texture(u_texture, v_tex_coord_ * u_tiling_factor) * u_color;
	color = v_color;
}