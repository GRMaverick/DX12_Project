#include "D3D12\Resources\IndexBufferResource.h"

namespace SysRenderer
{
	namespace D3D12
	{
		IndexBufferResource::IndexBufferResource( ID3D12Device* _pDevice, CommandList* _pCommandList, const UINT _sizeInBytes, const UINT _strideInBytes, const D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName )
		{
			if ( UploadResource( _pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData ) )
			{
				ZeroMemory( &m_ibView, sizeof(D3D12_INDEX_BUFFER_VIEW) );
				m_ibView.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
				m_ibView.SizeInBytes    = _sizeInBytes;
				m_ibView.Format         = DXGI_FORMAT_R32_UINT;
			}
		}

		IndexBufferResource::~IndexBufferResource( void )
		{
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferResource::GetView() const
		{
			return m_ibView;
		}
	}
}
