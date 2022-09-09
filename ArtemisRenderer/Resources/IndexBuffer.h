#ifndef __IndexBufferResource_h__
#define __IndexBufferResource_h__

#include "IBufferResource.h"

namespace ArtemisRenderer::Resources
{
	class IndexBuffer : public IBufferResource
	{
	public:
		IndexBuffer(ID3D12Device* _pDevice, Device::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr);
		~IndexBuffer(void);

		D3D12_INDEX_BUFFER_VIEW GetView();

	private:
		D3D12_INDEX_BUFFER_VIEW m_View;
	};
}

#endif // __IndexBufferResource_h__