#version 430 core

#define GLUS_PI 3.1415926535897932384626433832795

// see localSize = 16 in main.c.
layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform samplerCube u_cubeMap;

layout (rgba32f, binding = 1) uniform image2D u_textureLambert;

layout (rgba32f, binding = 2) uniform image2D u_textureCookTorrance;

layout (std430, binding = 3) buffer ScanVectors
{
	vec3 direction[];
	// Padding[]
} b_scanVectors;

uniform uint u_m;
uniform uint u_samples;
uniform float u_binaryFractionFactor;

uniform float u_roughness;

// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// see http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v2.pdf
// see Physically Based Rendering Chapter 13.6.1
vec3 microfacetWeightedSampling(vec2 e)
{
	float alpha = u_roughness * u_roughness;
	
	// Note: Polar Coordinates
	// x = sin(theta)*cos(phi)
	// y = sin(theta)*sin(phi)
	// z = cos(theta)
	
	float phi = 2.0 * GLUS_PI * e.y; 	
	float cosTheta = sqrt((1.0 - e.x) / (1.0 + (alpha*alpha - 1.0) * e.x));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta); 

	float x = sinTheta * cos(phi);
	float y = sinTheta * sin(phi);
	float z = cosTheta;

	return vec3(x, y, z);
}

// see Physically Based Rendering Chapter 13.6.1 and 13.6.3
vec3 cosineWeightedSampling(vec2 e)
{
	float x = sqrt(1.0 - e.x) * cos(2.0*GLUS_PI*e.y);
	float y = sqrt(1.0 - e.x) * sin(2.0*GLUS_PI*e.y);
	float z = sqrt(e.x);
	
	return vec3(x, y, z);
}

//
//
//

// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// see http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v2.pdf 
// see http://sirkan.iit.bme.hu/~szirmay/scook.pdf
vec4 lightCookTorrance(vec2 randomPoint, mat3 basis, vec3 N, vec3 V, float k)
{
	vec4 noColor = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 HtangentSpace = microfacetWeightedSampling(randomPoint);
	
	// Transform H to world space.
	vec3 H = basis * HtangentSpace;
	
	// Note: reflect takes incident vector.
	vec3 L = reflect(-V, H);
	
	float NdotL = dot(N, L);
	float NdotV = dot(N, V);
	
	// Lighted and visible
	if (NdotL > 0.0 && NdotV > 0.0)
	{
		return texture(u_cubeMap, L);;
	}
	
	return noColor;
}

//

// see http://www.scratchapixel.com/lessons/3d-advanced-lessons/things-to-know-about-the-cg-lighting-pipeline/what-is-a-brdf/
// see Physically Based Rendering Chapter 5.6.1, 13.2 and 13.6.3
// see Fundamentals of Computer Graphics Chapter 14.2 and 24.2
vec4 lightLambert(vec2 randomPoint, mat3 basis)
{
	vec3 LtangentSpace = cosineWeightedSampling(randomPoint);
	
	// Transform light ray to world space.
	vec3 L = basis * LtangentSpace;  

	return texture(u_cubeMap, L);
}

//
//
//

// see http://mathworld.wolfram.com/HammersleyPointSet.html
// see https://github.com/wdas/brdf/blob/master/src/shaderTemplates/brdfIBL.frag
vec2 hammersley(uint originalSample)
{
	uint revertSample;

	// Revert bits by swapping blockwise. Lower bits are moved up and higher bits down.
	revertSample = (originalSample << 16u) | (originalSample >> 16u);
	revertSample = ((revertSample & 0x00ff00ffu) << 8u) | ((revertSample & 0xff00ff00u) >> 8u);
	revertSample = ((revertSample & 0x0f0f0f0fu) << 4u) | ((revertSample & 0xf0f0f0f0u) >> 4u);
	revertSample = ((revertSample & 0x33333333u) << 2u) | ((revertSample & 0xccccccccu) >> 2u);
	revertSample = ((revertSample & 0x55555555u) << 1u) | ((revertSample & 0xaaaaaaaau) >> 1u);

	// Shift back, as only m bits are used.
	revertSample = revertSample >> (32 - u_m);

	return vec2(float(revertSample) * u_binaryFractionFactor, float(originalSample) * u_binaryFractionFactor);
}

void calculateBasis(out vec3 tangent, out vec3 bitangent, in vec3 normal)
{
	bitangent = vec3(0.0, 1.0, 0.0);

	float normalDotUp = dot(normal, bitangent);

	if (normalDotUp == 1.0)
	{
		bitangent = vec3(0.0, 0.0, -1.0);
	}
	else if (normalDotUp == -1.0)
	{
		bitangent = vec3(0.0, 0.0, 1.0);
	}
	
	tangent = cross(bitangent, normal);	
	bitangent = cross(normal, tangent);
} 

void main(void)
{
	vec4 colorLambert = vec4(0.0, 0.0, 0.0, 0.0);
	
	vec4 colorCookTorrance = vec4(0.0, 0.0, 0.0, 0.0);

	//	

	ivec2 dimension = imageSize(u_textureCookTorrance);

	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

	int pixelPos = storePos.x + storePos.y * dimension.x;
	
	//
	
	vec3 tangent;
	vec3 bitangent;
	vec3 normal = b_scanVectors.direction[pixelPos];
	
	calculateBasis(tangent, bitangent, normal);
	
	mat3 basis = mat3(tangent, bitangent, normal);
	
	//
	
	vec2 randomPoint;
	
	// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf Section Specular G
	float k = (u_roughness + 1.0) * (u_roughness + 1.0) / 8.0;
	
	for (uint sampleIndex = 0; sampleIndex < u_samples; sampleIndex++)
	{
		randomPoint = hammersley(sampleIndex);
	
		if (u_roughness == 0.0)
		{
			// Diffuse
			colorLambert += lightLambert(randomPoint, basis);
		}
		
		// Specular
		// see assumption N = V in Pre-Filtered Environment Map in Real Shading in Unreal Engine 4
		colorCookTorrance += lightCookTorrance(randomPoint, basis, normal, normal, k); 
	}
	
	if (u_roughness == 0.0)
	{
		imageStore(u_textureLambert, storePos, colorLambert / float(u_samples));
	}
	
	imageStore(u_textureCookTorrance, storePos, colorCookTorrance / float(u_samples));
}
