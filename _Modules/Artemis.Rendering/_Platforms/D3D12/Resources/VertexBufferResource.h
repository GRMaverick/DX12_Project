#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "IBufferResource.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class VertexBufferResource final : public Interfaces::IBufferResource
		{
		public:
			VertexBufferResource( ID3D12Device* _pDevice, CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );
			~VertexBufferResource( void ) override;

			D3D12_VERTEX_BUFFER_VIEW GetView() const;

		private:
			D3D12_VERTEX_BUFFER_VIEW m_vbView;
		};
	}
}

#endif // __VertexBufferResource_h__
