// Basic texture shader

#type vertex
#version 330 core
			
layout(location = 0) in vec3 position_;
layout(location = 1) in vec4 color_;
layout(location = 2) in vec2 tex_coord_;
layout(location = 3) in float tex_index_;
layout(location = 4) in float tiling_factor_;

uniform mat4 view_projection;

out vec4 v_color;
out vec2 v_tex_coord_;
out float v_tex_index_;
out float v_tiling_factor_;

void main() {
	v_color = color_;
	v_tex_coord_ = tex_coord_;
	v_tex_index_ = tex_index_;
	v_tiling_factor_ = tiling_factor_;
	gl_Position = view_projection * vec4(position_, 1.0);
}

#type fragment
#version 330 core
			
layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_tex_coord_;
in float v_tex_index_;
in float v_tiling_factor_;

uniform sampler2D u_textures[32];
			
void main() {
	color = texture(u_textures[int(v_tex_index_)], v_tex_coord_ * v_tiling_factor_) * v_color;
}