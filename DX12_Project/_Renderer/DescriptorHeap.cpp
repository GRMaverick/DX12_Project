#include "Defines.h"

#include "DescriptorHeap.h"

#include <assert.h>

using namespace Microsoft::WRL;

DescriptorHeap::DescriptorHeap(void)
{
	m_IncrementSize = 0;
	m_pDescriptorHeap = nullptr;
}
DescriptorHeap::~DescriptorHeap(void)
{
	if (m_pDescriptorHeap) m_pDescriptorHeap.Reset();
}

bool DescriptorHeap::Initialise(ComPtr<ID3D12Device> _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _type, UINT _numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	desc.Type = _type;
	desc.NumDescriptors = _numDescriptors;

	HRESULT hr = _pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pDescriptorHeap.GetAddressOf()));
	if (FAILED(hr))
	{
		assert(false && "Descriptor Heap Creation Failed");
		return false;
	}

	m_IncrementSize = _pDevice->GetDescriptorHandleIncrementSize(_type);

	return true;
}

UINT DescriptorHeap::GetIncrementSize(void) const
{
	return m_IncrementSize;
}
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUStartHandle(void) const
{
	return m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}