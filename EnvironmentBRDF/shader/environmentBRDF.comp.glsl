#version 430 core

#define GLUS_PI 3.1415926535897932384626433832795

// see localSize = 16 in main.c.
layout (local_size_x = 16, local_size_y = 16) in;

layout (rg32f, binding = 0) uniform image2D u_textureCookTorrance;

uniform uint u_m;
uniform uint u_samples;
uniform float u_binaryFractionFactor;

// see http://graphicrants.blogspot.de/2013/08/specular-brdf-reference.html
// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float geometricShadowingSchlickBeckmann(float NdotV, float k)
{
	return NdotV / (NdotV * (1.0 - k) + k);
}

// see http://graphicrants.blogspot.de/2013/08/specular-brdf-reference.html
// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float geometricShadowingSmith(float NdotL, float NdotV, float k)
{
	return geometricShadowingSchlickBeckmann(NdotL, k) * geometricShadowingSchlickBeckmann(NdotV, k);
}

// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// see http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v2.pdf
// see Physically Based Rendering Chapter 13.6.1
vec3 microfacetWeightedSampling(vec2 e, float roughness)
{
	float alpha = roughness * roughness;
	
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

//
//
//

// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
// see http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v2.pdf 
// see http://sirkan.iit.bme.hu/~szirmay/scook.pdf
vec2 integrateBrdfCookTorrance(vec2 randomPoint, vec3 N, vec3 V, float k, float roughness)
{
	vec2 noValue = vec2(0.0, 0.0);

	vec3 H = microfacetWeightedSampling(randomPoint, roughness);
	
	// Note: reflect takes incident vector.
	vec3 L = reflect(-V, H);
	
	float NdotL = dot(N, L);
	float NdotV = dot(N, V);
	float NdotH = dot(N, H);
	
	// Lighted and visible
	if (NdotL > 0.0 && NdotV > 0.0)
	{
		float VdotH = dot(V, H);

		// Geometric Shadowing
		float G = geometricShadowingSmith(NdotL, NdotV, k);
	
		//
		// Lo = BRDF * L * NdotL / PDF
		//
		// BRDF is D * F * G / (4 * NdotL * NdotV).
		// PDF is D * NdotH / (4 * VdotH).
		// D and 4 * NdotL are canceled out, which results in this formula: Lo = color * L * F * G * VdotH / (NdotV * NdotH)
		//
		// Using the approximation as 
		// seen on page 6 in Real Shading in Unreal Engine 4
		// L is stored in the cube map array, F is replaced and color is later used in the real-time BRDF shader.
				
		float colorFactor = G * VdotH / (NdotV * NdotH);
		
		// Note: Needed for robustness. With specific parameters, a NaN can be the result.
		if (isnan(colorFactor))
		{
			return noValue;
		}
		
		float fresnelFactor = pow(1.0 - VdotH, 5.0);
		
		// Scale to F0 (first sum)
		float scaleF0 = (1.0 - fresnelFactor) * colorFactor; 
		
		// Bias to F0 (second sum)
		float biasF0 = fresnelFactor * colorFactor;
				
		return vec2(scaleF0, biasF0);
	}
	
	return noValue;
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

void main(void)
{
	vec2 outputCookTorrance = vec2(0.0, 0.0);

	//	

	ivec2 dimension = imageSize(u_textureCookTorrance);

	ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);

	int pixelPos = storePos.x + storePos.y * dimension.x;
	
	//
		
	vec2 randomPoint;

	//

	float NdotV =  float(storePos.x) / float(dimension.x - 1);
	
	vec3 N = vec3(0.0, 0.0, 1.0);
	
	vec3 V;
	V.x = sqrt(1.0 - NdotV * NdotV);
	V.y = 0.0;
	V.z = NdotV;
	
	//
	
	float roughness =  float(storePos.y) / float(dimension.y - 1);
	
	// see http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf Section Specular G
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	
	//
	
	for (uint sampleIndex = 0; sampleIndex < u_samples; sampleIndex++)
	{
		randomPoint = hammersley(sampleIndex);
	
		// Specular
		outputCookTorrance += integrateBrdfCookTorrance(randomPoint, N, V, k, roughness); 
	}
	
	imageStore(u_textureCookTorrance, storePos, vec4(outputCookTorrance / float(u_samples), 0.0, 0.0));
}
