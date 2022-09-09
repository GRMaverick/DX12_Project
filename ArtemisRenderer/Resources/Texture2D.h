#ifndef __Texture2DResource_h__
#define __Texture2DResource_h__

#include <d3d12.h>

#include "IBufferResource.h"

#include "../Helpers/d3dx12.h"

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap;

	class Texture2D : public IBufferResource, public IGpuBufferResource
	{
	public:
		Texture2D(
			const wchar_t* _pWstrFilename,
			const bool _bIsDDS,
			DescriptorHeap* _pTargetSRVHeap,
			ID3D12Device* _pDevice,
			Device::CommandList* _pCmdList,
			const wchar_t* _pDebugName = nullptr
		);

		~Texture2D(void);

		UINT32 GetHeapIndex(void) { return m_HeapIndex; }

	private:
		UINT m_HeapIndex = 0;

		bool CreateFromDDS(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, Device::CommandList* _pCmdList);
		bool CreateFromWIC(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, Device::CommandList* _pCmdList);
	};
}

#endif __Texture2DResource_h__