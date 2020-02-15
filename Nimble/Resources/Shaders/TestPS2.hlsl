//Texture2D albedo : register(t0); SamplerState albedoSampler : register(s0);
//Texture2D normal : register(t1); SamplerState normalSampler : register(s1);
//Texture2D cocacola : register(t2);  SamplerState cocacolaSampler : register(s2);

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

float4 main(PixelInput input) : SV_Target
{
	//float4 a = albedo.Sample(albedoSampler, input.position.xy);
	//float4 n = normal.Sample(normalSampler, input.position.xy);
	return float4(input.color, 1.0);
}