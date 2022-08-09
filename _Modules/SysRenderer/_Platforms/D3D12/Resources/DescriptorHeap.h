#ifndef __DescriptorHeap_h__
#define __DescriptorHeap_h__

#include <d3d12.h>
#include <wrl.h>

namespace SysRenderer
{
	namespace D3D12
	{
		class DescriptorHeap
		{
			friend class DeviceD3D12;

		public:
			DescriptorHeap(void);
			~DescriptorHeap(void);

			bool Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_DESCRIPTOR_HEAP_TYPE _type, UINT _numDescriptors,
				D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName);

			void						Increment(void);
			UINT						GetFreeIndex(void);
			UINT						GetFreeIndexAndIncrement(void);
			ID3D12DescriptorHeap* GetHeap(void) const;
			UINT						GetIncrementSize(void) const;
			D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStartHandle(void) const;
			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStartHandle(void) const;

		private:
			UINT m_IncrementSize = 0;
			UINT m_MaxDescriptors = 0;
			UINT m_ActualDescriptors = 0;
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap = nullptr;
		};
	}
}
#endif //__DescriptorHeap_h__