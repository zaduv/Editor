#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D histopyramid;
uniform vec2 delta;

void main()
{
	FragColor = vec4(
		dot(vec4(1.0), floor(texture(histopyramid, TexCoords + delta.xx))),
		dot(vec4(1.0), floor(texture(histopyramid, TexCoords + delta.yx))),
		dot(vec4(1.0), floor(texture(histopyramid, TexCoords + delta.xy))),
		dot(vec4(1.0), floor(texture(histopyramid, TexCoords + delta.yy)))
	);
}