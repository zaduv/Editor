#version 330 core
out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 Bound;

void main()
{
	vec3 p = Position;
	vec3 n = Normal;

	vec2 pos = vec2(dot(n, p.zxy), dot(n, p.yzx)) + vec2(0.05);
	vec2 w = fwidth(pos);
	vec2 a = pos + 0.5 * w;                        
	vec2 b = pos - 0.5 * w;  
	vec2 i = (floor(a) + min(fract(a) * 10.0, 1.0) - floor(b) - min(fract(b) * 10.0, 1.0)) / (10.0 * w);

	vec2 r = step(Bound.xy, b) - step(Bound.zw + 0.1, a);
	vec2 s = step(Bound.xy - 0.5, pos) - step(Bound.zw + 0.5, pos);

	FragColor = vec4(0.0, 0.0, 0.0, max(i.x * i.y, max(i.x * r.y, i.y * r.x)) * s.x * s.y);
}