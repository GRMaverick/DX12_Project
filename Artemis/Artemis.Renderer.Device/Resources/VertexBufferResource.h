#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "Interfaces/IBufferResource.h"

namespace Artemis::Renderer::Device
{
	class CommandListDx12;

	class VertexBufferResource final : public Renderer::Interfaces::IBufferResource
	{
	public:
		VertexBufferResource( ID3D12Device* _pDevice, CommandListDx12* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );
		~VertexBufferResource( void ) override;

		D3D12_VERTEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_vbView;
	};
}

#endif // __VertexBufferResource_h__
