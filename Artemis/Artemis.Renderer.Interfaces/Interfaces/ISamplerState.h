#pragma once

namespace Artemis::Renderer::Interfaces
{
	enum class SamplerStateFilter : unsigned int
	{
		EPoint,
		ELinear,
		EAnisotropic,
		EMax
	};

	enum class SamplerStateWrapMode : unsigned int
	{
		EWrap = 1,
		EMirror,
		EClamp,
		EBorder,
		EMirrorOnce,
		EMax
	};

	enum class SamplerStateComparisonFunction : unsigned int
	{
		ENever = 1,
		ELess,
		EEqual,
		ELessEqual,
		EGreater,
		ENotEqual,
		EGreaterEqual,
		EAlways,
		EMax,
	};

	class ISamplerState
	{
	public:
		virtual ~ISamplerState( void );
	};
}
