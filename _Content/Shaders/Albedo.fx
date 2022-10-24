//
// Input/Outputs
//
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
	float2 Texture	: TEXCOORD;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 Texture : TEXCOORD;
};

//
// Constant Buffers / Resource Bindings
//
cbuffer PassCB : register(b0)
{
	float4x4	ViewProjection;
	float3		EyePosition;
};

cbuffer ObjectCB : register(b1)
{
	float4x4 World;
	float4	DiffuseColour;
	float4	AmbientColour;
	float4	SpecularColour;
};

//
// Entry Point
//
VSOutput MainVS(VSInput _input)
{
	VSOutput output;
	output.Position = mul(World, float4(_input.Position, 1.0f));
	output.Position = mul(ViewProjection, float4(output.Position.xyz, 1.0f));
	output.Texture = _input.Texture;
	return output;
}

//
// Constant Buffers / Resource Bindings
//
Texture2D Albedo : register(t0);
SamplerState AlbedoSampler : register(s0);

//
// Entry Point
//
float4 MainPS(VSOutput _input) : SV_TARGET
{
	return Albedo.Sample(AlbedoSampler, _input.Texture);
}