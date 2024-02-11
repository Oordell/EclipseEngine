// Basic texture shader

#type vertex
#version 450
			
layout(location = 0) in vec3 position_;
layout(location = 1) in vec4 color_;
layout(location = 2) in vec2 tex_coord_;
layout(location = 3) in float tex_index_;
layout(location = 4) in float tiling_factor_;
layout(location = 5) in int entity_id_;

uniform mat4 view_projection;

out vec4 v_color;
out vec2 v_tex_coord_;
out flat float v_tex_index_;
out float v_tiling_factor_;
out flat int v_entity_id_;

void main() {
	v_color = color_;
	v_tex_coord_ = tex_coord_;
	v_tex_index_ = tex_index_;
	v_tiling_factor_ = tiling_factor_;
	v_entity_id_ = entity_id_;
	gl_Position = view_projection * vec4(position_, 1.0);
}

#type fragment
#version 450
			
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_color;
in vec2 v_tex_coord_;
in flat float v_tex_index_;
in float v_tiling_factor_;
in flat int v_entity_id_;

uniform sampler2D u_textures[32];
			
void main() {
	// color = texture(u_textures[int(v_tex_index_)], v_tex_coord_ * v_tiling_factor_) * v_color;

	vec4 tex_color = v_color;
	switch(int(v_tex_index_)) {
		case 0: tex_color *= texture(u_textures[0], v_tex_coord_ * v_tiling_factor_); break;
		case 1: tex_color *= texture(u_textures[1], v_tex_coord_ * v_tiling_factor_); break;
		case 2: tex_color *= texture(u_textures[2], v_tex_coord_ * v_tiling_factor_); break;
		case 3: tex_color *= texture(u_textures[3], v_tex_coord_ * v_tiling_factor_); break;
		case 4: tex_color *= texture(u_textures[4], v_tex_coord_ * v_tiling_factor_); break;
		case 5: tex_color *= texture(u_textures[5], v_tex_coord_ * v_tiling_factor_); break;
		case 6: tex_color *= texture(u_textures[6], v_tex_coord_ * v_tiling_factor_); break;
		case 7: tex_color *= texture(u_textures[7], v_tex_coord_ * v_tiling_factor_); break;
		case 8: tex_color *= texture(u_textures[8], v_tex_coord_ * v_tiling_factor_); break;
		case 9: tex_color *= texture(u_textures[9], v_tex_coord_ * v_tiling_factor_); break;
		case 10: tex_color *= texture(u_textures[10], v_tex_coord_ * v_tiling_factor_); break;
		case 11: tex_color *= texture(u_textures[11], v_tex_coord_ * v_tiling_factor_); break;
		case 12: tex_color *= texture(u_textures[12], v_tex_coord_ * v_tiling_factor_); break;
		case 13: tex_color *= texture(u_textures[13], v_tex_coord_ * v_tiling_factor_); break;
		case 14: tex_color *= texture(u_textures[14], v_tex_coord_ * v_tiling_factor_); break;
		case 15: tex_color *= texture(u_textures[15], v_tex_coord_ * v_tiling_factor_); break;
		case 16: tex_color *= texture(u_textures[16], v_tex_coord_ * v_tiling_factor_); break;
		case 17: tex_color *= texture(u_textures[17], v_tex_coord_ * v_tiling_factor_); break;
		case 18: tex_color *= texture(u_textures[18], v_tex_coord_ * v_tiling_factor_); break;
		case 19: tex_color *= texture(u_textures[19], v_tex_coord_ * v_tiling_factor_); break;
		case 20: tex_color *= texture(u_textures[20], v_tex_coord_ * v_tiling_factor_); break;
		case 21: tex_color *= texture(u_textures[21], v_tex_coord_ * v_tiling_factor_); break;
		case 22: tex_color *= texture(u_textures[22], v_tex_coord_ * v_tiling_factor_); break;
		case 23: tex_color *= texture(u_textures[23], v_tex_coord_ * v_tiling_factor_); break;
		case 24: tex_color *= texture(u_textures[24], v_tex_coord_ * v_tiling_factor_); break;
		case 25: tex_color *= texture(u_textures[25], v_tex_coord_ * v_tiling_factor_); break;
		case 26: tex_color *= texture(u_textures[26], v_tex_coord_ * v_tiling_factor_); break;
		case 27: tex_color *= texture(u_textures[27], v_tex_coord_ * v_tiling_factor_); break;
		case 28: tex_color *= texture(u_textures[28], v_tex_coord_ * v_tiling_factor_); break;
		case 29: tex_color *= texture(u_textures[29], v_tex_coord_ * v_tiling_factor_); break;
		case 30: tex_color *= texture(u_textures[30], v_tex_coord_ * v_tiling_factor_); break;
		case 31: tex_color *= texture(u_textures[31], v_tex_coord_ * v_tiling_factor_); break;
	}
	color = tex_color;
	color2 = v_entity_id_;

}