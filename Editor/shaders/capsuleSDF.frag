#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec3 a;
uniform vec3 b;
uniform vec4 c;

uniform sampler2D dataFieldTex;

float sdCapsule(vec3 p, vec3 a, vec3 b, float r)
{
	vec3 pa = p - a, ba = b - a;
	float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
	return length(pa - ba * h) - r;
}

void main()
{
	vec3 p = vec3(mod(gl_FragCoord.x - 0.5, 16.0) * 4.0, floor((gl_FragCoord.x - 0.5) * 0.0625) + mod(gl_FragCoord.y - 0.5, 4.0) * 16.0, floor((gl_FragCoord.y - 0.5) * 0.25));

	vec4 d = vec4(
	sdCapsule(p + vec3(0.0, 0.0, 0.0), a, b, c.w),
	sdCapsule(p + vec3(1.0, 0.0, 0.0), a, b, c.w),
	sdCapsule(p + vec3(2.0, 0.0, 0.0), a, b, c.w),
	sdCapsule(p + vec3(3.0, 0.0, 0.0), a, b, c.w));

	FragColor = min(texture(dataFieldTex, TexCoords), d);
}