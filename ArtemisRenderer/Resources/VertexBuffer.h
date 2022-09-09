#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "IBufferResource.h"

namespace ArtemisRenderer::Resources
{
	class VertexBuffer : public IBufferResource
	{
	public:
		VertexBuffer(ID3D12Device* _pDevice, Device::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr);
		~VertexBuffer(void);

		D3D12_VERTEX_BUFFER_VIEW GetView();

	private:
		D3D12_VERTEX_BUFFER_VIEW m_View;
	};
}

#endif // __VertexBufferResource_h__
