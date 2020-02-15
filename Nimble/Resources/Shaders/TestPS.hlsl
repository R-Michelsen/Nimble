struct PixelInput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

struct PixelOutput
{
	float4 albedo : SV_TARGET0;
	float4 normal : SV_TARGET1;
};

PixelOutput main(PixelInput input)
{
	PixelOutput output;
	output.albedo = float4(1.0f, 1.0f, 0.0f, 1.0f);
	output.normal = float4(0.0f, 1.0f, 1.0f, 1.0f);
	return output;
}