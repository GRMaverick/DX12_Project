#include "VertexBufferResourceDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	VertexBufferResourceDx12::VertexBufferResourceDx12(const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName)
	{
		if (UploadResource(_pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData))
		{
			ZeroMemory(&m_vbView, sizeof(D3D12_VERTEX_BUFFER_VIEW));
			m_vbView.BufferLocation = m_gpuBuffer->GetGPUVirtualAddress();
			m_vbView.SizeInBytes = _sizeInBytes;
			m_vbView.StrideInBytes = _strideInBytes;
		}
	}

	VertexBufferResourceDx12::~VertexBufferResourceDx12(void)
	{
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBufferResourceDx12::GetView() const
	{
		return m_vbView;
	}
}
