#ifndef __DescriptorHeap_h__
#define __DescriptorHeap_h__

#include <d3d12.h>
#include <wrl.h>

class DescriptorHeap
{
public:
	DescriptorHeap(void);
	~DescriptorHeap(void);

	bool Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _type, UINT _numDescriptors);

	UINT						GetIncrementSize(void) const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle(void) const;
		
private:
	UINT m_IncrementSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap = nullptr;;
};
#endif //__DescriptorHeap_h__