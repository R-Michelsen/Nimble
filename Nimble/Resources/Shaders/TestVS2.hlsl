cbuffer CBUF : register(b0)
{
	matrix mvp;
}

struct VertexInput
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	output.position = mul(mvp, float4(input.position, 1.0));
	output.color = input.color;

	return output;
}