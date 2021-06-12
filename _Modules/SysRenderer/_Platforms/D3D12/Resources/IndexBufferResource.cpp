#include "D3D12\Resources\IndexBufferResource.h"

IndexBufferResource::IndexBufferResource(ID3D12Device* _pDevice, CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName)
{
	if (UploadResource(_pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData))
	{
		ZeroMemory(&m_View, sizeof(D3D12_INDEX_BUFFER_VIEW));
		m_View.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
		m_View.SizeInBytes = _sizeInBytes;
		m_View.Format = DXGI_FORMAT_R32_UINT;
	}
}

IndexBufferResource::~IndexBufferResource(void)
{
}

D3D12_INDEX_BUFFER_VIEW IndexBufferResource::GetView()
{
	return m_View;
}
