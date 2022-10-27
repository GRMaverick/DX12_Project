#ifndef __IndexBufferResource_h__
#define __IndexBufferResource_h__

#include "Aliases.h"

namespace Artemis::Renderer::Device
{
	class CommandListDx12;

	class IndexBufferResource final : public IABufferResource
	{
	public:
		IndexBufferResource( ID3D12Device* _pDevice, IACommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );
		~IndexBufferResource( void ) override;

		D3D12_INDEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_INDEX_BUFFER_VIEW m_ibView;
	};
}

#endif // __IndexBufferResource_h__
