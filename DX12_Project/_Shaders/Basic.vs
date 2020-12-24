struct ModelViewProjection
{
	matrix MVP;
};

struct VSInput
{
	float3 Position : POSITION;
	float3 Color : COLOR;
};

struct VSOutput
{
	float4 Position : SV_Position;
	float4 Color : COLOR;
};

ConstantBuffer<ModelViewProjection> MVP_CB : register(b0);

VSOutput main(VSInput _input)
{
	VSOutput output;
	output.Position = mul(MVP_CB.MVP, float4(_input.Position, 1.0f));
	output.Color = float4(_input.Color, 1.0f);

	return output;
}