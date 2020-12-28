#include "RootSignatures/Albedo.rs"

//
// Constant Buffers / Resource Bindings
//
struct Object
{
	float4x4 World;
};
ConstantBuffer<Object> ObjectCB : register(b1);

struct Pass
{
	float4x4 ViewProjection;
};
ConstantBuffer<Pass> PassCB : register(b0);

//
// Input/Outputs
//
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

//
// Entry Point
//
[RootSignature(RootSignatureDef)]
VSOutput main(VSInput _input)
{
	VSOutput output;
	output.Position = mul(ObjectCB.World, float4(_input.Position, 1.0f));
	output.Position = mul(PassCB.ViewProjection, float4(output.Position.xyz, 1.0f));
	output.Texture = _input.Texture;
	return output;
}