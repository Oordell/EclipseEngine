// Basic texture shader

#type vertex
#version 450 core
			
layout(location = 0) in vec3 position_;
layout(location = 1) in vec4 color_;
layout(location = 2) in vec2 tex_coord_;
layout(location = 3) in float tex_index_;
layout(location = 4) in float tiling_factor_;
layout(location = 5) in int entity_id_;

layout(std140, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

struct VertexOutput {
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main() {
	Output.Color = color_;
	Output.TexCoord = tex_coord_;
	Output.TexIndex = tex_index_;
	Output.TilingFactor = tiling_factor_;
	v_EntityID = entity_id_;

	gl_Position = u_ViewProjection * vec4(position_, 1.0);
}

#type fragment
#version 450 core
			
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct VertexOutput {
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];
			
void main() {
	vec4 tex_color = Input.Color;

	switch(int(Input.TexIndex)) {
		case 0: tex_color *= texture(u_Textures[0], Input.TexCoord * Input.TilingFactor); break;
		case 1: tex_color *= texture(u_Textures[1], Input.TexCoord * Input.TilingFactor); break;
		case 2: tex_color *= texture(u_Textures[2], Input.TexCoord * Input.TilingFactor); break;
		case 3: tex_color *= texture(u_Textures[3], Input.TexCoord * Input.TilingFactor); break;
		case 4: tex_color *= texture(u_Textures[4], Input.TexCoord * Input.TilingFactor); break;
		case 5: tex_color *= texture(u_Textures[5], Input.TexCoord * Input.TilingFactor); break;
		case 6: tex_color *= texture(u_Textures[6], Input.TexCoord * Input.TilingFactor); break;
		case 7: tex_color *= texture(u_Textures[7], Input.TexCoord * Input.TilingFactor); break;
		case 8: tex_color *= texture(u_Textures[8], Input.TexCoord * Input.TilingFactor); break;
		case 9: tex_color *= texture(u_Textures[9], Input.TexCoord * Input.TilingFactor); break;
		case 10: tex_color *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: tex_color *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: tex_color *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: tex_color *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: tex_color *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: tex_color *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
		case 16: tex_color *= texture(u_Textures[16], Input.TexCoord * Input.TilingFactor); break;
		case 17: tex_color *= texture(u_Textures[17], Input.TexCoord * Input.TilingFactor); break;
		case 18: tex_color *= texture(u_Textures[18], Input.TexCoord * Input.TilingFactor); break;
		case 19: tex_color *= texture(u_Textures[19], Input.TexCoord * Input.TilingFactor); break;
		case 20: tex_color *= texture(u_Textures[20], Input.TexCoord * Input.TilingFactor); break;
		case 21: tex_color *= texture(u_Textures[21], Input.TexCoord * Input.TilingFactor); break;
		case 22: tex_color *= texture(u_Textures[22], Input.TexCoord * Input.TilingFactor); break;
		case 23: tex_color *= texture(u_Textures[23], Input.TexCoord * Input.TilingFactor); break;
		case 24: tex_color *= texture(u_Textures[24], Input.TexCoord * Input.TilingFactor); break;
		case 25: tex_color *= texture(u_Textures[25], Input.TexCoord * Input.TilingFactor); break;
		case 26: tex_color *= texture(u_Textures[26], Input.TexCoord * Input.TilingFactor); break;
		case 27: tex_color *= texture(u_Textures[27], Input.TexCoord * Input.TilingFactor); break;
		case 28: tex_color *= texture(u_Textures[28], Input.TexCoord * Input.TilingFactor); break;
		case 29: tex_color *= texture(u_Textures[29], Input.TexCoord * Input.TilingFactor); break;
		case 30: tex_color *= texture(u_Textures[30], Input.TexCoord * Input.TilingFactor); break;
		case 31: tex_color *= texture(u_Textures[31], Input.TexCoord * Input.TilingFactor); break;
	}
	color = tex_color;
	color2 = v_EntityID;

}