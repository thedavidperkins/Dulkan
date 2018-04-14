#version 450

layout (location = 0) in vec4 app_position;

//layout (push_constant) uniform my_matrices_t {
//	mat4 modelview;
//	mat4 perspective;
//} my_matrices;

void main() {
	//gl_Position = my_matrices.perspective * my_matrices.modelview * app_position;
	gl_Position = app_position;
}