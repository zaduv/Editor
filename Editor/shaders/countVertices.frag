#version 330 core
out vec4 FragColor;

uniform sampler2D dataFieldTex;
uniform sampler2D triTableTex;
uniform vec3 min;
uniform vec3 max;

// Evaluates the 2d index value in the simulated 3D texture from the 3D position
vec4 index2D(vec3 p)
{
	return texture(dataFieldTex, vec2(mod(p.x * 0.25 + p.y * 16.0, 256.0) + 0.5, floor(p.y * 0.0625) + p.z * 4.0 + 0.5) * 0.00390625);
}

// Check if the point is inside the box
float insideBox3D(vec3 v, vec3 bottomLeft, vec3 topRight)
{
	vec3 s = step(bottomLeft, v) - step(topRight, v);
	return s.x * s.y * s.z; 
}

// Get vertex i position within current marching cube
vec3 vertex(float i)
{
	return vec3(step(0.5, mod(i, 4)) * step(mod(i, 4), 2.5), step(1.5, mod(i, 4)), step(3.5, i));
}

void main()
{
	// Determine current position
	vec3 p = vec3(mod(gl_FragCoord.x - 0.5, 16.0) * 4.0, floor((gl_FragCoord.x - 0.5) * 0.0625) + mod(gl_FragCoord.y - 0.5, 4.0) * 16.0, floor((gl_FragCoord.y - 0.5) * 0.25));

	// Determine the index with the vertices inside of the surface
	vec4 c = 
	1.0 * step(0.0, index2D(p + vertex(0.0))) +
	2.0 * step(0.0, vec4(index2D(p + vertex(0.0)).yzw, index2D(p + vertex(0.0) + vec3(4.0, 0.0, 0.0)).x)) +
	4.0 * step(0.0, vec4(index2D(p + vertex(3.0)).yzw, index2D(p + vertex(3.0) + vec3(4.0, 0.0, 0.0)).x)) +
	8.0 * step(0.0, index2D(p + vertex(3.0))) +
	16.0 * step(0.0, index2D(p + vertex(4.0))) +
	32.0 * step(0.0, vec4(index2D(p + vertex(4.0)).yzw, index2D(p + vertex(4.0) + vec3(4.0, 0.0, 0.0)).x)) +
	64.0 * step(0.0, vec4(index2D(p + vertex(7.0)).yzw, index2D(p + vertex(7.0) + vec3(4.0, 0.0, 0.0)).x)) +
	128.0 * step(0.0, index2D(p + vertex(7.0)));

	// Determine the number of vertices
	FragColor = vec4(
	texture(triTableTex, vec2(mod(c.x, 16.0), floor(c.x * 0.0625)) * 0.015625).g * insideBox3D(p + vec3(0.0, 0.0, 0.0), min, max) + 1.0 / 256.0 * c.x,
	texture(triTableTex, vec2(mod(c.y, 16.0), floor(c.y * 0.0625)) * 0.015625).g * insideBox3D(p + vec3(1.0, 0.0, 0.0), min, max) + 1.0 / 256.0 * c.y,
	texture(triTableTex, vec2(mod(c.z, 16.0), floor(c.z * 0.0625)) * 0.015625).g * insideBox3D(p + vec3(2.0, 0.0, 0.0), min, max) + 1.0 / 256.0 * c.z,
	texture(triTableTex, vec2(mod(c.w, 16.0), floor(c.w * 0.0625)) * 0.015625).g * insideBox3D(p + vec3(3.0, 0.0, 0.0), min, max) + 1.0 / 256.0 * c.w);
}