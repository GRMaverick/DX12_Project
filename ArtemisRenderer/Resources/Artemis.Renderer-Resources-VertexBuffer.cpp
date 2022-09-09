module;

#include "../Resources/VertexBuffer.h"

module Artemis.Renderer:Resources;

namespace ArtemisRenderer::Resources
{
	VertexBuffer::VertexBuffer(ID3D12Device* _pDevice, Device::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName)
	{
		if (UploadResource(_pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData))
		{
			ZeroMemory(&m_View, sizeof(D3D12_VERTEX_BUFFER_VIEW));
			m_View.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
			m_View.SizeInBytes = _sizeInBytes;
			m_View.StrideInBytes = _strideInBytes;
		}
	}

	VertexBuffer::~VertexBuffer(void)
	{
	}

	D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetView()
	{
		return m_View;
	}
}