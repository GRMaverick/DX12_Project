#pragma once

#include "_Interfaces\ISamplerState.h"

#include "Defines.h"

class SamplerStateD3D12 : public ISamplerState
{
public:
	SamplerStateD3D12(void);
	~SamplerStateD3D12(void);

	bool Initialise(ID3D12Device* _pDevice, DescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc);

	unsigned int GetHeapIndex(void) const;
	D3D12_SAMPLER_DESC GetState(void) const;

private:
	unsigned int m_HeapIndex;
	D3D12_SAMPLER_DESC m_SamplerState;
};