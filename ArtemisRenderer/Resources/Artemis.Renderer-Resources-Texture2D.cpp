module;

//#include "TextureLoader.h"
//#include <WICTextureLoader.h>

//#include "SysMemory/include/MemoryGlobalTracking.h"

module Artemis.Renderer:Resources;

import "Texture2D.h";

import "Helpers/Defines.h";

import "Device/RenderDevice.h";
import "Device/CommandList.h";

import "Resources/DescriptorHeap.h";

namespace ArtemisRenderer::Resources
{
	Texture2D::Texture2D(const wchar_t* _pWstrFilename, const bool _bIsDDS, DescriptorHeap* _pTargetSRVHeap,
		ID3D12Device* _pDevice, Device::CommandList* _pCmdList, const wchar_t* _pDebugName)
	{
		m_HeapIndex = _pTargetSRVHeap->GetFreeIndexAndIncrement();

		if (_bIsDDS)
		{
			CreateFromDDS(_pWstrFilename, _pDevice, _pCmdList);
		}
		else
		{
			CreateFromWIC(_pWstrFilename, _pDevice, _pCmdList);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = m_GPUBuffer->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_GPUBuffer->GetDesc().MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		m_hCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pTargetSRVHeap->GetCPUStartHandle());
		m_hCpuHandle.Offset(m_HeapIndex, _pTargetSRVHeap->GetIncrementSize());
		_pDevice->CreateShaderResourceView(m_GPUBuffer, &srvDesc, m_hCpuHandle);

		wchar_t pCPUDebugName[256];
		wchar_t pGPUDebugName[256];

		wsprintf(pCPUDebugName, L"%s_%s", _pDebugName, L"CPU");
		wsprintf(pGPUDebugName, L"%s_%s", _pDebugName, L"GPU");

		if (m_CPUBuffer)
			m_CPUBuffer->SetName(pCPUDebugName);

		m_GPUBuffer->SetName(pGPUDebugName);
	}

	Texture2D::~Texture2D(void)
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

	bool Texture2D::CreateFromDDS(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, Device::CommandList* _pCmdList)
	{
		//VALIDATE_D3D(CreateDDSTextureFromFile12(_pDevice, _pCmdList, _pWstrFilename, m_GPUBuffer, m_CPUBuffer));

		auto descCPU = m_CPUBuffer->GetDesc();
		//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureCPU, m_CPUBuffer,
		//	descCPU.Width * descCPU.Height * descCPU.DepthOrArraySize
		//);

		auto descGPU = m_GPUBuffer->GetDesc();
		//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureGPU, m_GPUBuffer,
		//	descGPU.Width * descGPU.Height * descGPU.DepthOrArraySize
		//);

		return true;
	}

	bool Texture2D::CreateFromWIC(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, Device::CommandList* _pCmdList)
	{
		D3D12_SUBRESOURCE_DATA srData;
		std::unique_ptr<uint8_t[]> decodedData;
		//if (FAILED(LoadWICTextureFromFile(_pDevice, _pWstrFilename, m_GPUBuffer, decodedData, srData)))
			return false;

		D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC uploadRdBuffer = CD3DX12_RESOURCE_DESC::Buffer(srData.SlicePitch);
		VALIDATE_D3D(_pDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadRdBuffer,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_CPUBuffer)
		));

		_pCmdList->UpdateSubresource(m_GPUBuffer, m_CPUBuffer, 0, 0, 1, &srData);

		CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_GPUBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		_pCmdList->ResourceBarrier(1, &tgtBarrier);

		auto descCPU = m_CPUBuffer->GetDesc();
		//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureCPU, m_CPUBuffer,
		//	descCPU.Width * descCPU.Height * descCPU.DepthOrArraySize
		//);

		auto descGPU = m_GPUBuffer->GetDesc();
		//MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureGPU, m_GPUBuffer,
		//	descGPU.Width * descGPU.Height * descGPU.DepthOrArraySize
		//);

		return true;
	}
}