#pragma once

namespace SysRenderer
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

	namespace Interfaces
	{
		class ISamplerState
		{
		public:
			virtual ~ISamplerState( void )
			{
			};
		};
	}
}
