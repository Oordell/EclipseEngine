// Flat color shader

#type vertex
#version 330 core
			
layout(location = 0) in vec3 position_;

uniform mat4 view_projection;
uniform mat4 transform;

void main() {
	gl_Position = view_projection * transform * vec4(position_, 1.0);
}

#type fragment
#version 330 core
			
layout(location = 0) out vec4 color;

uniform vec4 u_color;
			
void main() {
	color = u_color;
}