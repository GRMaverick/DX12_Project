#include "IndexBufferResourceDx12.h"

#include "Interfaces/ICommandList.h"

namespace Artemis::Renderer::Device::Dx12
{
	IndexBufferResourceDx12::IndexBufferResourceDx12(const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, const unsigned int _sizeInBytes, const unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName )
	{
		m_ibView = {};
		if ( UploadResource( _pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData ) )
		{
			ZeroMemory( &m_ibView, sizeof(D3D12_INDEX_BUFFER_VIEW) );
			m_ibView.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
			m_ibView.SizeInBytes    = _sizeInBytes;
			m_ibView.Format         = DXGI_FORMAT_R32_UINT;
		}
	}

	IndexBufferResourceDx12::~IndexBufferResourceDx12( void )
	{
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferResourceDx12::GetView() const
	{
		return m_ibView;
	}
}
