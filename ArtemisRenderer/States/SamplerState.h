#ifndef __SamplerState_h__
#define __SamplerState_h__

#include <d3d12.h>

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap;
}

namespace ArtemisRenderer::States
{
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

	class SamplerState
	{
	public:
		SamplerState(void);
		~SamplerState(void);

		bool Initialise(ID3D12Device* _pDevice, ArtemisRenderer::Resources::DescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc);

		unsigned int GetHeapIndex(void) const;
		D3D12_SAMPLER_DESC GetState(void) const;

	private:
		unsigned int m_HeapIndex;
		D3D12_SAMPLER_DESC m_SamplerState;
	};
}

#endif // __SamplerState_h__