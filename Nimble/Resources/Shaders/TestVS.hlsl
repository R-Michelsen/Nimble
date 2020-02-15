RWStructuredBuffer<float> testBuffer : register(u2);

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

	testBuffer[0] = 1.0f;
	testBuffer[1] = 2.0f;
	testBuffer[2] = 3.0f;
	testBuffer[3] = 4.0f;
	testBuffer[4] = 5.0f;

	output.position = float4(input.position, 1.0);
	output.color = input.color;
	
	return output;
}