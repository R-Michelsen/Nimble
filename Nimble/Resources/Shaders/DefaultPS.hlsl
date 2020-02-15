Texture2D shadowMap				: register(t0); SamplerState shadowSampler					: register(s0);
Texture2D baseColorMap			: register(t1); SamplerState baseColorSampler				: register(s1);
Texture2D metallicRoughnessMap	: register(t2); SamplerState metallicRoughnessSampler		: register(s2);
Texture2D normalMap				: register(t3); SamplerState normalSampler					: register(s3);
Texture2D occlusionMap			: register(t4); SamplerState occlusionSampler				: register(s4);
Texture2D emissiveMap			: register(t5); SamplerState emissiveSampler				: register(s5);

cbuffer MaterialProperties : register(b1)
{
	float4 baseColorFactor;
	float4 emissiveFactor;
	float metallicFactor;
	float roughnessFactor;
	float alphaMask;
	float alphaMaskCutoff;
	int baseColorTextureIndex;
	int metallicRoughnessTextureIndex;
	int normalTextureIndex;
	int occlusionTextureIndex;
	int emissiveTextureIndex;
}

struct PixelInput
{
	float4 positionCS : SV_POSITION;
	float3 positionWS : POSITION;
	float3 normal : NORMAL;
	float2 UV0 : TEXCOORD;
	float2 UV1 : TEXCOORD1;
};

float3 GetNormal(PixelInput input)
{
	float3 tangentNormal = normalMap.Sample(normalSampler, normalTextureIndex == 0 ? input.UV0 : input.UV1).xyz * 2.0f - 1.0f;

	float3 q1 = ddx(input.positionWS);
	float3 q2 = ddy(input.positionWS);
	float2 st1 = ddx(input.UV0);
	float2 st2 = ddy(input.UV0);

	float3 T = normalize(q1 * st2.y - q2 * st1.y);
	float3 N = normalize(input.normal);
	float3 B = -normalize(cross(N, T));
	float3x3 TBN = float3x3(T, B, N);

	return normalize(mul(TBN, tangentNormal));
}

float4 main(PixelInput input) : SV_Target
{
	float4 baseColour = (baseColorTextureIndex > -1) ?
		baseColorMap.Sample(baseColorSampler, baseColorTextureIndex == 0 ? input.UV0 : input.UV1) : baseColorFactor;

	float3 normal = (normalTextureIndex > -1) ? GetNormal(input) : input.normal;

	return baseColour;
}