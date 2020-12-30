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
	float3		EyePosition;
	float4x4	ViewProjection;
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
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 diffuse = { 0.0f, 0.0f, 0.0f };
	float3 ambient = { 0.0f, 0.0f, 0.0f };
	float3 specular = { 0.0f, 0.0f, 0.0f };

	float3 normal = normalize(_input.Normal);

	float3 toEye = PassCB.EyePosition - _input.PosW.xyz;
	float distToEye = length(toEye);
	toEye = normalize(toEye);

	float4 texColor = Albedo.Sample(AlbedoSampler, _input.Texture);
	//float4 normalMap = txDiffuse[1].Sample(samLinear, _input.Texture);

	//input.Tangent = normalize(input.Tangent - dot(_input.Tangent, _input.NormW) * _input.NormW);

	//float3 bitangent = cross(input.NormW, input.Tangent);

	//if (dot(cross(input.NormW, input.Tangent), bitangent) < 0.0f)
	//{
	//	input.Tangent = input.Tangent * -1.0f;
	//}

	//float3x3 tangentSpace = float3x3(_input.Tangent, bitangent, input.NormW);
	//input.NormW = normalize(mul(normalMap, tangentSpace));

	float3 lightPos = LightCB.Position;
	float3 reflection = reflect(lightPos, normal);
	float diffuseAmount = max(dot(lightPos, normal), 0.0f);
	float specularAmount = pow(max(dot(reflection, toEye), 0.0f), LightCB.SpecularPower);

	diffuse = diffuseAmount * (ObjectCB.DiffuseColour * LightCB.DiffuseColour).rgb;
	ambient = (ObjectCB.AmbientColour * LightCB.AmbientColour).rgb;

	if (all(diffuse) <= 0.0f)
	{
		specular = (0.0f, 0.0f, 0.0f);
	}
	else
	{
		specular = specularAmount * (ObjectCB.SpecularColour * LightCB.SpecularColour).rgb;
	}

	color.rgb = clamp(texColor.rgb * (diffuse + ambient) + specular, 0, 1).rgb;

	return color;
}