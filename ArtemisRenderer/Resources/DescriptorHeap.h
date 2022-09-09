#ifndef __DescriptorHeap_h__
#define __DescriptorHeap_h__

#include <d3d12.h>

namespace ArtemisRenderer::Device
{
	class RenderDevice;
}

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap
	{
		friend class ArtemisRenderer::Device::RenderDevice;

	public:
		DescriptorHeap(void);
		~DescriptorHeap(void);

		bool Initialise(
			ID3D12Device* _pDevice, 
			D3D12_DESCRIPTOR_HEAP_TYPE _type,
			UINT _numDescriptors,
			D3D12_DESCRIPTOR_HEAP_FLAGS _flags, 
			const wchar_t* _pDebugName);

		void						Increment(void);
		UINT						GetFreeIndex(void);
		UINT						GetFreeIndexAndIncrement(void);
		ID3D12DescriptorHeap*		GetHeap(void) const;
		UINT						GetIncrementSize(void) const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle(void) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartHandle(void) const;

	private:
		UINT m_IncrementSize = 0;
		UINT m_MaxDescriptors = 0;
		UINT m_ActualDescriptors = 0;
		ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;
	};
}
#endif //__DescriptorHeap_h__