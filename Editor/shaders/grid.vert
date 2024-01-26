#version 330 core
out vec3 Position;
out vec3 Normal;
out vec4 Bound;

uniform mat4 MVP;
uniform vec3 origin;
uniform vec3 min;
uniform vec3 max;

void main()
{
	float x = float(((uint(gl_VertexID) + 2u) / 3u)%2u); 
    float y = float(((uint(gl_VertexID) + 1u) / 3u)%2u); 

	vec3 p1 = step(origin.yzx, origin.xyz);
	vec3 p2 = step(origin.zxy, origin.xyz);
	vec3 p3 = vec3(step(gl_VertexID, 5.5), step(5.5, gl_VertexID) * step(gl_VertexID, 11.5), step(11.5, gl_VertexID));

	vec3 n = 
	p3.x * (p1 * p2) +
	p3.y * (p1 + p2 - 2 * p1 * p2) +
	p3.z * (vec3(1.0) - p2 - p1 + p1 * p2);

	float a = step(0.0, dot(0.5 * (min + max) - origin, n));
	vec3 w = max - min + 0.2;

	Position =
	n.x * mix(min - vec3(0.1) + vec3(0.0, x, y) * w, max + vec3(0.1) - vec3(0.0, y, x) * w, a) +
	n.y * mix(min - vec3(0.1) + vec3(y, 0.0, x) * w, max + vec3(0.1) - vec3(x, 0.0, y) * w, a) +
	n.z * mix(min - vec3(0.1) + vec3(x, y, 0.0) * w, max + vec3(0.1) - vec3(y, x, 0.0) * w, a);

	Bound = vec4(
	n.x * min.zy + n.y * min.xz + n.z * min.yx,
	n.x * max.zy + n.y * max.xz + n.z * max.yx);

	Normal = n;
	n *= (a * 2 - 1);

	gl_Position = MVP * vec4(Position, 1.0);
}