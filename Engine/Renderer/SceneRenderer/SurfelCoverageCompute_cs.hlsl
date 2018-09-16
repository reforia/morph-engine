#include "Common.hlsli"
#include "Surfel.hlsli"
#include "Lighting.hlsli"

#define SurfelPlacement_RootSig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
     RootSig_Common \
		"DescriptorTable(SRV(t10, numDescriptors = 5), visibility = SHADER_VISIBILITY_ALL)," \
		"DescriptorTable(UAV(u0, numDescriptors = 3), visibility = SHADER_VISIBILITY_ALL)," \
    "StaticSampler(s0, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_ALL),"



Texture2D<float4> gTexAlbedo:   register(t10);
Texture2D<float4> gTexNormal:   register(t11);
Texture2D<float4> gTexPosition: register(t12);
Texture2D gTexDepth: register(t13);
StructuredBuffer<vertex_t> gVerts: register(t14);

RWStructuredBuffer<surfel_t> uSurfels: register(u0);
RWStructuredBuffer<uint> uNumSurfels: register(u1);
RWTexture2D<float4> uSurfelCoverage: register(u2);


float dimishingAdd(float a, float b) {
	return 1.f - (1.f-a)*(1.f-b);
}

float coverageAt(uint2 pix) {
	float coverage = 0.f;
	uint numSurfel = uNumSurfels[0];
	for(uint i = 0; i < numSurfel; i++) {
		float3 position = gTexPosition[pix].xyz;
		float3 normal = gTexNormal[pix].xyz * 2.f - float3(1.f, 1.f, 1.f);

		coverage += isCovered(position, normal, uSurfels[i]);
	}
	return coverage;
}

[RootSignature(SurfelPlacement_RootSig)]
[numthreads(32, 32, 1)]
void main( uint3 threadId : SV_DispatchThreadID)
{
	uint2 pix = threadId.xy;

	RETURN_IF_OUT_TEX(pix, gTexNormal);

	uSurfelCoverage[pix] = float4(coverageAt(pix), 0, 0, 1.f);
}