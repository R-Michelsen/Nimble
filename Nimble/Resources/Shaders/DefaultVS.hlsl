cbuffer ModelMatrices : register(b0)
{
	matrix model;
	matrix mvp;
}

struct VertexInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 UV0 : TEXCOORD;
	float2 UV1 : TEXCOORD1;
};

struct PixelInput
{
	float4 positionCS : SV_POSITION;
	float3 positionWS : POSITION;
	float3 normal : NORMAL;
	float2 UV0 : TEXCOORD;
	float2 UV1 : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output;
	output.positionCS = mul(mvp, float4(input.position, 1.0));
	output.positionWS = mul(model, float4(input.position, 1.0)).xyz;
	output.normal = input.normal;
	output.UV0 = input.UV0;
	output.UV1 = input.UV1;

	return output;
}