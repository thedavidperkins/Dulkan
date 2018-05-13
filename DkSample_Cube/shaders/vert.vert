#version 450

layout (location = 0) in vec4 app_position;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform my_matrix_t
{
	mat4 perspective[16];
} my_matrix;

void main() {
	gl_Position = my_matrix.perspective[gl_InstanceIndex] * app_position;
	outColor = inColor;
}