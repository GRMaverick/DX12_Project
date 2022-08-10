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
	float4 PosH		: SV_Position;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
	float2 Texture	: TEXCOORD;
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
	float4	MaterialDiffuse;
	float4	MaterialAmbient;
	float4	MaterialSpecular;
};

cbuffer LightCB : register(b2)
{
	float4	LightDiffuse;
	float4	LightAmbient;
	float4	LightSpecular;
	float3	LightPosition;
	float	LightSpecularPower;
};

cbuffer SpotlightCB : register(b3)
{
	float4	SpotlightDiffuse;
	float4	SpotlightAmbient;
	float4	SpotlightSpecular;
	float3	SpotlightPosition;
	float	SpotlightSpecularPower;

	float	SpotlightTheta;
	float	SpotlightPhi;
	float	SpotlightFalloff;
};

//
// Entry Point
//
VSOutput MainVS(VSInput _input)
{
	VSOutput output;

	output.PosW = mul(World, float4(_input.Position, 1.0f)).xyz;
	output.PosH = mul(ViewProjection, float4(output.PosW.xyz, 1.0f));

	output.Texture = _input.Texture;

	output.Normal = mul(World, float4(_input.Normal, 1.0f)).xyz;
	output.Normal = normalize(output.Normal);
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
	float4 color = float4(0.5f, 0.0f, 0.0f, 0.0f);
	float4 texColor = Albedo.Sample(AlbedoSampler, _input.Texture);

	// Ambient Component
	float4 ambient = LightAmbient * MaterialAmbient;

	// Diffuse Component
	float3 lightPos = LightPosition;
	float3 normal = normalize(_input.Normal);
	float diffuseAmount = saturate(dot(lightPos, normal));
	float4 diffuse = LightDiffuse * MaterialDiffuse;

	// Specular Component
	float3 viewNormalized = normalize(EyePosition - _input.PosW);
	float reflection = normalize(2 * diffuseAmount * normal - normalize(lightPos));
	float specularAmount = pow(saturate(dot(reflection, viewNormalized)), LightSpecularPower);

	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (LightSpecularPower > 0.0f)
	{
		specular.rgb = specularAmount * (MaterialSpecular * LightSpecular).rgb;
	}

	return clamp(texColor * (specular + (diffuse * diffuseAmount) + ambient),0,1);
}