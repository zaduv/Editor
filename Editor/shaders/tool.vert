#version 330 core

out vec3 direction;

uniform mat4 MVP;

uniform vec3 p1;
uniform vec3 p2;
uniform vec3 p3;

uniform vec3 n1;
uniform vec3 n2;
uniform vec3 n3;

uniform vec3 origin;

void main()
{
	float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u) * 4.4 - 2.2; 
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u) * 4.4 - 2.2; 

	vec3 p = p1 * step(gl_VertexID, 5.5) + p2 * step(5.5, gl_VertexID) * step(gl_VertexID, 11.5) + p3 * step(11.5, gl_VertexID);
	vec3 n = n1 * step(gl_VertexID, 5.5) + n2 * step(5.5, gl_VertexID) * step(gl_VertexID, 11.5) + n3 * step(11.5, gl_VertexID);

	vec3 u = normalize(
	vec3(0.0, n.z, -n.y) * step(n.x, n.y) * step(n.x, n.z) +
	vec3(-n.z, 0.0, n.x) * step(n.y, n.x) * step(n.y, n.z) +
	vec3(-n.y, n.x, 0.0) * step(n.z, n.x) * step(n.z, n.y));

	vec3 v = cross(n, u);

	vec3 position = mix(p + u * x + v * y, p + u * y + v * x, step(0.0, dot(p - origin, n)));
	direction = position - origin;

	gl_Position = MVP * vec4(position, 1.0);
}