#version 450

layout (location = 0) in vec4 app_position;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec4 inNormal;
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;

layout (set = 0, binding = 0) uniform my_matrix_t
{
	mat4 perspective[16];
} my_matrix;

layout (set = 0, binding = 1) uniform my_matrix_normal_t
{
	mat4 perspective[16];
} my_matrix_normal;

void main() {
	gl_Position = my_matrix.perspective[gl_InstanceIndex] * app_position;
	outColor = inColor;
	outNormal = my_matrix_normal.perspective[gl_InstanceIndex] * inNormal;
}