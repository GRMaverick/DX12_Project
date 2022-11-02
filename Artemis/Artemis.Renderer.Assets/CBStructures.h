#pragma once

#include <DirectXMath.h>

struct Pass
{
	DirectX::XMMATRIX ViewProjection;
	DirectX::XMFLOAT3 EyePosition;
};

struct Light
{
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Position;
	float SpecularPower;
};

struct Spotlight
{
	Light LightData;
	
	float Theta;
	float Phi;
	float Falloff;
};

struct Material
{
	DirectX::XMFLOAT4 Diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT4 Ambient = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 Specular = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct Object
{
	DirectX::XMMATRIX World;
	Material Material;
};
