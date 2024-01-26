#version 330 core
out vec4 FragColor;

in vec3 direction;

uniform vec3 p1;
uniform vec3 p2;
uniform vec3 p3;

uniform vec3 n1;
uniform vec3 n2;
uniform vec3 n3;

uniform vec3 origin;

void main()
{
	vec3 d = abs(vec3(
	length(origin - dot(origin - p1, n1) / dot(direction, n1) * direction - p1) - 2.0,
	length(origin - dot(origin - p2, n2) / dot(direction, n2) * direction - p2) - 2.0, 
	length(origin - dot(origin - p3, n3) / dot(direction, n3) * direction - p3) - 2.0)) - 0.1;

	vec3 a = max(1.0 - clamp(0.5 - d / fwidth(d), 0.0, 1.0), 1.0 - step(d, vec3(0.1)));
	vec3 b = step(1.0, a);

	gl_FragDepth = 0.0;
	FragColor = vec4(mix(vec3(b.y * b.z, b.x * b.z, b.x * b.y), vec3(a.y * a.z, a.x * a.z, a.x * a.y) * (1.0 - a), step(1.5, dot(1.0 - b, vec3(1.0)))), (1.0 - a.x * a.y * a.z));
}