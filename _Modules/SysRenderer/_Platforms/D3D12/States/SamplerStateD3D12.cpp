#include "SamplerStateD3D12.h"

#include "D3D12\Resources\DescriptorHeap.h"

SamplerStateD3D12::SamplerStateD3D12(void)
{
	m_HeapIndex = 0;

	m_SamplerState.MinLOD = 0;
	m_SamplerState.MaxLOD = D3D12_FLOAT32_MAX;
	m_SamplerState.MipLODBias = 0.0f;
	m_SamplerState.MaxAnisotropy = 1;
}

SamplerStateD3D12::~SamplerStateD3D12(void)
{

}

bool SamplerStateD3D12::Initialise(ID3D12Device* _pDevice, DescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc)
{
	m_SamplerState.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)_eWrap;
	m_SamplerState.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)_eWrap;
	m_SamplerState.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)_eWrap;
	m_SamplerState.ComparisonFunc = (D3D12_COMPARISON_FUNC)_eCompFunc;

	switch (_eFilter)
	{
		case SamplerStateFilter::Linear:
			m_SamplerState.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			break;
	}

	m_HeapIndex = _pDescHeap->GetFreeIndexAndIncrement();

	CD3DX12_CPU_DESCRIPTOR_HANDLE sampHandle(_pDescHeap->GetCPUStartHandle());
	sampHandle.Offset(m_HeapIndex, _pDescHeap->GetIncrementSize());

	_pDevice->CreateSampler(&m_SamplerState, sampHandle);

	return true;
}

unsigned int SamplerStateD3D12::GetHeapIndex(void) const
{
	return m_HeapIndex;
}

D3D12_SAMPLER_DESC SamplerStateD3D12::GetState(void) const
{
	return m_SamplerState;
}