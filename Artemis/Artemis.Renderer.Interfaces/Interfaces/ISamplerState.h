#pragma once

namespace Artemis::Renderer::Interfaces
{
    class IGraphicsDevice;
    class IDescriptorHeap;

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
        virtual ~ISamplerState(void);		
		virtual bool Initialise(const IGraphicsDevice* _pDevice, IDescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc) = 0;
	};
}
