#version 330 core
    
out vec4 Position;
out vec4 Normal;
out vec4 Color;

uniform sampler2D dataFieldTex;
uniform sampler2D normalFieldTex;
uniform sampler2D triTableTex;
uniform sampler2D histopyramid;

uniform mat4 MVP;

// Get vertex i position within current marching cube
vec3 vertex(float i)
{
	return vec3(step(0.5, mod(i, 4)) * step(mod(i, 4), 2.5), step(1.5, mod(i, 4)), step(3.5, i));
}

// Get edge i position within current marching cube
vec2 edge(float i)
{
	return step(i, 3.5) * vec2(i, mod(i + 1, 4)) + step(3.5, i) * step(i, 7.5) * vec2(i, mod(i - 3, 4) + 4) + step(7.5, i) * vec2(i - 8, i - 4);
}

// Get value at current point
float cubeVal(vec3 p)
{
	return dot(texture(dataFieldTex, vec2(mod(floor(p.x * 0.25) + p.y * 16.0, 256.0) + 0.5, floor(p.y * 0.0625) + p.z * 4.0 + 0.5) * 0.00390625), vec4(equal(vec4(mod(p.x, 4.0)), vec4(0, 1, 2, 3))));
}

void main() 
{
	// The key index is determined from the integer index of the vertex
	float key_ix = gl_VertexID;

	// Start traversal in the center of the top element texel
	vec2 texpos = vec2(0.5);

	// Texel shift incremental offsets, one element per interval, these are updated during traversal
	vec4 delta_x = vec4(-0.5, 0.5, -0.5, 0.25);
	vec4 delta_y = vec4(0.0, -0.5, 0.0, 0.25);

	// Variables to traverse the pyramid
	vec4 sums, hist, mask;

	// First, traverse the upper levels containing only integers
	for (int i = 0; i < 8; i++) 
	{
		// Fetch sub-pyramid sums for the four sub-pyramids
		sums = texture2DLod(histopyramid, texpos, float(8 - i));
		hist = sums;
		hist.w += hist.z;
		hist.zw += hist.yy;
		hist.yzw += hist.xxx;
		mask = vec4(lessThan(vec4(key_ix), hist));

		// Combine mask with delta_x and delta_y to shift texcoord
		texpos += vec2(dot(mask, delta_x), dot(mask, delta_y));

		// Subtract start of interval from key index
		key_ix -= dot(sums.xyz, vec3(1.0) - mask.xyz);

		// Scale texcoord shifts for next level
		delta_x *= 0.5;
		delta_y *= 0.5;
	}

	// Traverse the last level containing the case
	vec4 raw  = texture2DLod( histopyramid, texpos, 0.0 );
	sums = floor(raw);
	hist = sums;
	hist.w   += hist.z;
	hist.zw  += hist.yy;
	hist.yzw += hist.xxx;
	mask = vec4( lessThan( vec4(key_ix), hist ) );
	float nib = dot(vec4(mask), vec4(-1.0,-1.0,-1.0, 3.0));
	texpos   += vec2( dot( mask, delta_x ), dot( mask, delta_y ) );
	key_ix   -= dot( sums.xyz, vec3(1.0)-mask.xyz );
	float val = fract( dot( raw, vec4(equal(vec4(nib),vec4(0,1,2,3))) ) ) * 256.0;

	// Get texture coordinates
	vec2 uv = floor(texpos * 256);

	// Get vertex i position within current marching cube
	vec3 p = vec3(mod(uv.x, 16.0) * 4.0 + nib, floor(uv.x * 0.0625) + mod(uv.y, 4.0) * 16.0, floor(uv.y * 0.25));

	// Extract the corresponding MC code
	float i = texture(triTableTex, vec2(mod(16.0 * val + key_ix, 64.0), floor((16.0 * val + key_ix) * 0.015625)) * 0.015625).r;

	// Find two edges
	vec2 edge = edge(i);
	vec3 a = p + vertex(edge.x);
	vec3 b = p + vertex(edge.y);
	vec2 c = vec2(mod(a.x + a.y * 64.0, 512.0) + 0.5, floor(a.y * 0.125) + a.z * 8.0 + 0.5) / 512.0;
	vec2 d = vec2(mod(b.x + b.y * 64.0, 512.0) + 0.5, floor(b.y * 0.125) + b.z * 8.0 + 0.5) / 512.0;

	// Find factor
	float pin = cubeVal(a);
	float pout = cubeVal(b);
	float e = (0.0 - pin) / (pout - pin);

	// Interpolate
	Position = vec4(mix(a, b, e), 0.0);
	Normal = mix(texture(normalFieldTex, c), texture(normalFieldTex, d), e);
	Color = vec4(1.0, 0.0, 0.0, 0.0);

	gl_Position = MVP * vec4(Position.xyz, 1.0);
}