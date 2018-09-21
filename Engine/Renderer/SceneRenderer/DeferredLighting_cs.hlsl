#include "Common.hlsli"
#include "Surfel.hlsli"
#include "Lighting.hlsli"

#define DeferredLighting_RootSig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
     RootSig_Common \
		"DescriptorTable(SRV(t10, numDescriptors = 6), visibility = SHADER_VISIBILITY_ALL)," \
		"DescriptorTable(UAV(u0, numDescriptors = 4), visibility = SHADER_VISIBILITY_ALL)," \
    "StaticSampler(s0, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_ALL),"



Texture2D<float4> gTexAlbedo:   register(t10);
Texture2D<float4> gTexNormal:   register(t11);
Texture2D<float4> gTexPosition: register(t12);
Texture2D gTexDepth: register(t13);
StructuredBuffer<vertex_t> gVerts: register(t14);
Texture2D<float4> gTexAO: register(t15);


RWStructuredBuffer<surfel_t> uSurfels: register(u0);
RWStructuredBuffer<uint> uNumSurfels: register(u1);
RWStructuredBuffer<SurfelBucketInfo> uSurfelBucket: register(u2);
RWTexture2D<float4> uTexScene: register(u3);


float3 Ambient(uint2 pix) {
	return gTexAO[pix].xyz;
}

Contact trace(Ray ray) {
	uint vertCount, stride;
	gVerts.GetDimensions(vertCount, stride);

	Contact contact;
	contact.t = 1e6;
	contact.valid = false;

	for(uint i = 0; i < vertCount; i+=3) {
		Contact c = triIntersection(gVerts[i].position.xyz, gVerts[i+1].position.xyz, gVerts[i+2].position.xyz, gVerts[i].position.w, ray);
		bool valid = c.valid && (c.t < contact.t) && (c.t >= 0.f);	 // equal to zero avoid the fail intersaction in the corner	edge
		if(valid)	{
			contact = c;
		}
	}

	return contact;
}

float3 computeDiffuse(float3 surfacePosition, float3 surfaceNormal) {
	Ray ray;

	float maxDist = length(gLight.position - surfacePosition);
	ray.direction = normalize(gLight.position - surfacePosition);

	ray.position = surfacePosition + surfaceNormal * 0.0001f;

	Contact c = trace(ray);


	if(c.valid && c.t < maxDist) return float3(0,0,0);
	
	return Diffuse(surfacePosition, surfaceNormal, float3(1, 1, 1), gLight);

}

float3 PhongLighting(uint2 pix)
{
	float3 surfacePosition = gTexPosition[pix].xyz;
	float3 surfaceNormal = gTexNormal[pix].xyz * 2.f - 1.f;
	float3 surfaceColor = gTexAlbedo[pix].xyz;
	float3 eyePosition;
	{
		float4 eye = mul(inverse(view), mul(inverse(projection), float4(0,0,0,1.f)));
		eyePosition = eye.xyz / eye.w;
	}
  float3 ambient = Ambient(pix);

	const float SPECULAR_AMOUNT = 1.f;
	const float SPECULAR_POWER = 2.f;
  float3 diffuse = computeDiffuse(surfacePosition, surfaceNormal);
  float3 specular = Specular(surfacePosition, surfaceNormal, 
														 normalize(eyePosition - surfacePosition), SPECULAR_AMOUNT, SPECULAR_POWER, gLight);

	float3 indirect = float3(0,0,0);
	uint count = uNumSurfels[0];

	float total = 0;
	for(uint i = 0; i < count; i++) {
	/*
		float d = distance(surfacePosition, uSurfels[i].position);
		if(d > .5f) continue;
		float factor = max(0, dot(normalize(uSurfels[i].position - surfacePosition), normalize(surfaceNormal)));
		// factor = dot(surfaceNormal, uSurfels[i].normal) >= 0 ? factor : 0;
		total += factor > 0 ? 1 : 0;
		float scale = Attenuation(1.f, .5f, float3(1.f, 0, 1.f));
		float atten = ( Attenuation(1.f, d, float3(1.f, 0, 1.f)) - scale ) / (1 - scale);
		atten = atten * atten;
		factor = factor * atten;
	 */
		float d = distance(surfacePosition, uSurfels[i].position);

		float weight = 1 / ((d*d) / (SURFEL_RADIUS * SURFEL_RADIUS)+ SURFEL_RADIUS + 1);
		float iscovered = saturate((dot(surfaceNormal, uSurfels[i].normal))) * weight;
		total += weight;
		indirect =	indirect + 
								(uSurfels[i].indirectLighting) * iscovered;
	}
		
	indirect /= total;


	// only A ray will enter the pixel, so the color need to divide by 2PI
  float3 color = (diffuse / 3.14159f + 2 * indirect) * surfaceColor / ( 2 * 3.141592f )/* + specular*/;

	const float GAMMA = 1.f / 2.1;
	color =  pow(color, float3(GAMMA, GAMMA, GAMMA)) * ambient;

  return clamp(color, float3(0.f, 0.f, 0.f), float3(1.f, 1.f, 1.f));
}

[RootSignature(DeferredLighting_RootSig)]
[numthreads(32, 32, 1)]
void main( uint3 threadId: SV_DispatchThreadID )
{
	uint2 pix = threadId.xy;

	float3 direct = 0;
	float3 indirect = 0;
	uint count = uNumSurfels[0];

	float3 color = PhongLighting(pix);
	uTexScene[pix] = float4(color, 1.f);
}

