#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "IBufferResource.h"

class VertexBufferResource : public IBufferResource
{
public:
	VertexBufferResource(ID3D12Device* _pDevice, CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr);
	~VertexBufferResource(void);

	D3D12_VERTEX_BUFFER_VIEW GetView();

private:
	D3D12_VERTEX_BUFFER_VIEW m_View;
};

#endif // __VertexBufferResource_h__
