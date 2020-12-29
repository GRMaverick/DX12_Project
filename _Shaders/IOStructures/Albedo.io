//
// Input/Outputs
//
struct VSInput
{
	float3 Position : POSITION;
	//float3 Normal : NORMAL;
	float2 Texture : TEXCOORD;
};

struct VSOutput
{
	float4 Position : SV_Position;
	float2 Texture : TEXCOORD;
};