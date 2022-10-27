#include "D3D12\Resources\VertexBufferResource.h"

namespace SysRenderer
{
	namespace D3D12
	{
		VertexBufferResource::VertexBufferResource( ID3D12Device* _pDevice, CommandList* _pCommandList, const UINT _sizeInBytes, const UINT _strideInBytes, const D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName )
		{
			if ( UploadResource( _pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData ) )
			{
				ZeroMemory( &m_vbView, sizeof(D3D12_VERTEX_BUFFER_VIEW) );
				m_vbView.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
				m_vbView.SizeInBytes    = _sizeInBytes;
				m_vbView.StrideInBytes  = _strideInBytes;
			}
		}

		VertexBufferResource::~VertexBufferResource( void )
		{
		}

		D3D12_VERTEX_BUFFER_VIEW VertexBufferResource::GetView() const
		{
			return m_vbView;
		}
	}
}
