struct ModelViewProjection
{
	matrix MVP;
};

struct VSInput
{
	float3 Position : POSITION;
	float2 Texture : TEXCOORD;
};

struct VSOutput
{
	float4 Position : SV_Position;
	float2 Texture : TEXCOORD;
};

ConstantBuffer<ModelViewProjection> MVP_CB : register(b0);

VSOutput main(VSInput _input)
{
	VSOutput output;
	output.Position = mul(MVP_CB.MVP, float4(_input.Position, 1.0f));
	output.Texture = _input.Texture;
	return output;
}