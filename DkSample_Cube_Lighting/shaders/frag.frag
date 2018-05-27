#version 450

layout( location = 0 ) in vec4 inColor;
layout( location = 1 ) in vec4 normal;
layout( location = 0 ) out vec4 frag_color;

void main() {
    vec3 lightDir = vec3(0.f, .25f, .5f);
	vec3 norm = normalize(vec3(normal));
	vec3 light = vec3(1.f, 1.f, 0.f);
	float amb = 1.0f;

	frag_color = vec4((1 - amb) * vec3(inColor) + amb * dot(norm, lightDir) * light, 1.f);
}