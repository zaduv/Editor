#version 330 core
out vec2 TexCoords;

void main()
{
	TexCoords = vec2(float((gl_VertexID & 1) << 2), float((gl_VertexID & 2) << 1)) * 0.5;
	gl_Position = vec4(-1.0 + float((gl_VertexID & 1) << 2), -1.0 + float((gl_VertexID & 2) << 1), 0.0, 1.0);
}