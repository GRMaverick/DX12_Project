#pragma once

enum class SamplerStateFilter : unsigned int
{
	Point,
	Linear,
	Anisotropic,
	Max
};

enum class SamplerStateWrapMode : unsigned int
{
	Wrap = 1,
	Mirror,
	Clamp,
	Border,
	MirrorOnce,
	Max
};

enum class SamplerStateComparisonFunction : unsigned int
{
	Never = 1,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,
	Max,
};

class ISamplerState
{
public:
	virtual ~ISamplerState(void) {};
};