#version 450

layout (location = 0) in vec4 app_position;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;

layout (std430, push_constant) uniform my_matrix_t {
	mat4 perspective;
} my_matrix;

void main() {
	gl_Position = my_matrix.perspective * app_position;
	outColor = inColor;
}