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

bool DescriptorHeap::Initialise(ComPtr<ID3D12Device> _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _type, UINT _numDescriptors,
	D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	desc.Type = _type;
	desc.NumDescriptors = _numDescriptors;
	desc.Flags = _flags;

	VALIDATE_D3D(_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pDescriptorHeap.GetAddressOf())));
	m_IncrementSize = _pDevice->GetDescriptorHandleIncrementSize(_type);
	m_MaxDescriptors = _numDescriptors;

	m_pDescriptorHeap->SetName(_pDebugName);

	return true;
}

void					DescriptorHeap::Increment()
{
	assert((m_ActualDescriptors <= m_MaxDescriptors - 1) && "Too many SRVs allocated on this heap");
	m_ActualDescriptors++;
}

UINT32 DescriptorHeap::GetFreeIndex(void)
{
	return m_ActualDescriptors;
}

UINT32 DescriptorHeap::GetFreeIndexAndIncrement(void)
{
	UINT32 ret = m_ActualDescriptors;
	Increment();
	return ret;
}

ID3D12DescriptorHeap* DescriptorHeap::GetHeap(void) const
{
	return m_pDescriptorHeap.Get();
}

UINT DescriptorHeap::GetIncrementSize(void) const
{
	return m_IncrementSize;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUStartHandle(void) const
{
	return m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUStartHandle(void) const
{
	return m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}