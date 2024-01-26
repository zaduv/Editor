#version 330 core
out vec4 FragColor;

uniform sampler2D dataFieldTex;

// Get vertex i value within current marching cube
float cubeVal(vec3 p)
{
	return dot(texture(dataFieldTex, vec2(mod(floor(p.x * 0.25) + p.y * 16.0, 256.0) + 0.5, floor(p.y * 0.0625) + p.z * 4.0 + 0.5) * 0.00390625), vec4(equal(vec4(mod(p.x, 4.0)), vec4(0, 1, 2, 3))));
}

// Get vertex i normal within current marching cube
vec3 cubeNormal(vec3 p)
{
	return vec3(
		cubeVal(p + vec3(1.0, 0.0, 0.0)) - cubeVal(p - vec3(1.0, 0.0, 0.0)),
		cubeVal(p + vec3(0.0, 1.0, 0.0)) - cubeVal(p - vec3(0.0, 1.0, 0.0)),
		cubeVal(p + vec3(0.0, 0.0, 1.0)) - cubeVal(p - vec3(0.0, 0.0, 1.0))
	);
}

void main()
{
	vec3 p = vec3(mod(gl_FragCoord.x - 0.5, 64.0), floor((gl_FragCoord.x - 0.5) / 64.0) + mod(gl_FragCoord.y - 0.5, 8.0) * 8.0, floor((gl_FragCoord.y - 0.5) * 0.125));

	FragColor = vec4(cubeNormal(p), 1.0);
}