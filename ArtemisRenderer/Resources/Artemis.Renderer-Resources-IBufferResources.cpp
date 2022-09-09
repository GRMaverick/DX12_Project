module;

#include "IBufferResource.h"

#include "../Device/CommandList.h"

//#include "SysMemory/include/MemoryGlobalTracking.h"

#include <assert.h>

module Artemis.Renderer:Resources;

//using namespace SysMemory;

namespace ArtemisRenderer::Resources
{
	IBufferResource::~IBufferResource(void)
	{
		if (m_CPUBuffer)
		{
			//MemoryGlobalTracking::RecordExplicitDellocation(m_CPUBuffer.Get());
			m_CPUBuffer->Release();
		}

		if (m_GPUBuffer)
		{
			//MemoryGlobalTracking::RecordExplicitDellocation(m_GPUBuffer.Get());
			m_GPUBuffer->Release();
		}
	}

	bool IBufferResource::UploadResource(ID3D12Device* _pDevice, Device::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName)
	{
		HRESULT hr = S_OK;

		UINT bufferSize = _sizeInBytes;

		D3D12_RESOURCE_DESC dRdBuffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, _flags);
		D3D12_HEAP_PROPERTIES dHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		hr = _pDevice->CreateCommittedResource(&dHeapProperties, D3D12_HEAP_FLAG_NONE, &dRdBuffer, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_GPUBuffer));
		if (FAILED(hr))
		{
			assert(false && "Destination Buffer Setup Failed");
			return false;
		}

		//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eGeometryCPU, m_CPUBuffer.Get(),
		//	bufferSize
		//);

		if (_pData)
		{
			ID3D12Resource* pIntermediate = nullptr;

			D3D12_HEAP_PROPERTIES iHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			D3D12_RESOURCE_DESC iRdBuffer = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			hr = _pDevice->CreateCommittedResource(&iHeapProperties, D3D12_HEAP_FLAG_NONE, &iRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_CPUBuffer));
			if (FAILED(hr))
			{
				assert(false && "Upload Buffer Setup Failed");
				return false;
			}

			//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eGeometryGPU, m_CPUBuffer.Get(),
			//	bufferSize
			//);

			D3D12_SUBRESOURCE_DATA srData = {};
			ZeroMemory(&srData, sizeof(D3D12_SUBRESOURCE_DATA));
			srData.pData = _pData;
			srData.RowPitch = bufferSize;
			srData.SlicePitch = srData.RowPitch;

			_pCommandList->UpdateSubresource(m_GPUBuffer, m_CPUBuffer, 0, 0, 1, &srData);
		}

		return true;
	}

	void IBufferResource::SetCPUBuffer(ID3D12Resource* _pBuffer)
	{
		m_CPUBuffer = _pBuffer;
	}
	void IBufferResource::SetGPUBuffer(ID3D12Resource* _pBuffer)
	{
		m_GPUBuffer = _pBuffer;
	}

	ID3D12Resource* IBufferResource::GetCPUBuffer(void)
	{
		return m_CPUBuffer;
	}
	ID3D12Resource* IBufferResource::GetGPUBuffer(void)
	{
		return m_GPUBuffer;
	}
}