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
struct Pass
{
	float4x4	ViewProjection;
	float3		EyePosition;
};
ConstantBuffer<Pass> PassCB : register(b0);

struct Object
{
	float4x4 World;
	float4	DiffuseColour;
	float4	AmbientColour;
	float4	SpecularColour;
};
ConstantBuffer<Object> ObjectCB : register(b1);

struct Light
{
	float4	DiffuseColour;
	float4	AmbientColour;
	float4	SpecularColour;
	float3	Position;
	float	SpecularPower;
};
ConstantBuffer<Light> LightCB : register(b2);

//
// Entry Point
//
VSOutput VertexShader(VSInput _input)
{
	VSOutput output;

	output.PosW = mul(ObjectCB.World, float4(_input.Position, 1.0f)).xyz;
	output.PosH = mul(PassCB.ViewProjection, float4(output.PosW.xyz, 1.0f));

	output.Texture = _input.Texture;

	output.Normal = mul(ObjectCB.World, float4(_input.Normal, 1.0f)).xyz;
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
float4 PixelShader(VSOutput _input) : SV_TARGET
{
	float4 color = float4(0.5f, 0.0f, 0.0f, 0.0f);
	float4 texColor = Albedo.Sample(AlbedoSampler, _input.Texture);

	// Ambient Component
	float4 ambient = LightCB.AmbientColour * ObjectCB.AmbientColour;

	// Diffuse Component
	float3 lightPos = LightCB.Position;
	float3 normal = normalize(_input.Normal);
	float diffuseAmount = saturate(dot(lightPos, normal));
	float4 diffuse = LightCB.DiffuseColour * ObjectCB.DiffuseColour;

	// Specular Component
	float3 viewNormalized = normalize(PassCB.EyePosition - _input.PosW);
	float reflection = normalize(2 * diffuseAmount * normal - normalize(lightPos));
	float specularAmount = pow(saturate(dot(reflection, viewNormalized)), LightCB.SpecularPower);

	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (LightCB.SpecularPower > 0.0f)
	{
		specular.rgb = specularAmount * (ObjectCB.SpecularColour * LightCB.SpecularColour).rgb;
	}

	return clamp(texColor * (specular + (diffuse * diffuseAmount) + ambient),0,1);
}