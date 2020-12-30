#pragma once

#include <DirectXMath.h>

struct Pass
{
	DirectX::XMFLOAT3 EyePosition;
	DirectX::XMMATRIX ViewProjection;
};

struct Light
{
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT3 Position;
	float SpecularPower;
};

struct Material
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
};

struct Object
{
	DirectX::XMMATRIX World;
	Material Material;
};
