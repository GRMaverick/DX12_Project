#pragma once

#include <DirectXMath.h>

#include "Vectors/Vector3.h"
#include "Vectors/Vector4.h"

struct Pass
{
	DirectX::XMMATRIX ViewProjection;
	Artemis::Maths::Vector3 EyePosition;
};

struct Light
{
	Artemis::Maths::Vector4 Diffuse;
	Artemis::Maths::Vector4 Ambient;
	Artemis::Maths::Vector4 Specular;
	Artemis::Maths::Vector3 Position;
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
	Artemis::Maths::Vector4 Diffuse = Artemis::Maths::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	Artemis::Maths::Vector4 Ambient = Artemis::Maths::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	Artemis::Maths::Vector4 Specular = Artemis::Maths::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
};

struct Object
{
	DirectX::XMMATRIX World;
	Material Material;
};
