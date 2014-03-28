#version 430 core

// see localSize = 16 in main.c.
layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform samplerCube u_cubeMap;

layout (rgba32f, binding = 1) uniform image2D u_texture;

layout (std430, binding = 2) buffer ScanVectors
{
	vec3 direction[];
	// Padding[]
} b_scanVectors;

uniform int u_m;
uniform int u_samples;

uniform float u_roughness;

void main(void)
{
	ivec2 dimension = textureSize(u_cubeMap, 0);

	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

	int pixelPos = storePos.x + storePos.y * dimension.x;
	
	// TODO
	
	vec4 color = texture(u_cubeMap, b_scanVectors.direction[pixelPos]);
	
	imageStore(u_texture, storePos, color);
}
