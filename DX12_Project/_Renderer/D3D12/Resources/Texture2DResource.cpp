#include "Defines.h"

#include "Texture2DResource.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Resources\DescriptorHeap.h"

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "SysMemory/include/MemoryGlobalTracking.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysMemory;

Texture2DResource::Texture2DResource(const wchar_t* _pWstrFilename, const bool _bIsDDS, DescriptorHeap* _pTargetSRVHeap,
	ID3D12Device* _pDevice, CommandList* _pCmdList, const char* _pDebugName)
{
	m_HeapIndex = _pTargetSRVHeap->GetFreeIndex();
	_pTargetSRVHeap->Increment();

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
	srvDesc.Format = m_CPUBuffer->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = m_CPUBuffer->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(_pTargetSRVHeap->GetCPUStartHandle());
	srvHandle.Offset(m_HeapIndex, _pTargetSRVHeap->GetIncrementSize());
	_pDevice->CreateShaderResourceView(m_GPUBuffer.Get(), &srvDesc, srvHandle);

	wchar_t pCPUDebugName[256];
	wchar_t pGPUDebugName[256];

	wsprintf(pCPUDebugName, L"%s_%s", _pDebugName, L"CPU");
	wsprintf(pGPUDebugName, L"%s_%s", _pDebugName, L"GPU");

	m_CPUBuffer->SetName(pCPUDebugName);
	m_GPUBuffer->SetName(pGPUDebugName);
}

Texture2DResource::~Texture2DResource(void)
{
	if (m_CPUBuffer)
	{
		MemoryGlobalTracking::RecordExplicitDellocation(m_CPUBuffer.Get());
		m_CPUBuffer.Reset();
	}

	if (m_GPUBuffer)
	{
		MemoryGlobalTracking::RecordExplicitDellocation(m_GPUBuffer.Get());
		m_GPUBuffer.Reset();
	}
}

bool Texture2DResource::CreateFromDDS(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList)
{
	VALIDATE_D3D(CreateDDSTextureFromFile12(_pDevice, _pCmdList, _pWstrFilename, m_GPUBuffer, m_CPUBuffer));
	
	auto descCPU = m_CPUBuffer->GetDesc();	
	MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureCPU, m_CPUBuffer.Get(),
		descCPU.Width * descCPU.Height * descCPU.DepthOrArraySize
	);

	auto descGPU = m_GPUBuffer->GetDesc();
	MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureGPU, m_GPUBuffer.Get(),
		descGPU.Width * descGPU.Height * descGPU.DepthOrArraySize
	);
	
	return true;
}

bool Texture2DResource::CreateFromWIC(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList)
{
	D3D12_SUBRESOURCE_DATA srData;
	std::unique_ptr<uint8_t[]> decodedData;
	if (FAILED(LoadWICTextureFromFile(_pDevice, _pWstrFilename, m_GPUBuffer.GetAddressOf(), decodedData, srData)))
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

	_pCmdList->UpdateSubresource(m_GPUBuffer.Get(), m_CPUBuffer.Get(), 0, 0, 1, &srData);

	auto descCPU = m_CPUBuffer->GetDesc();
	MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureCPU, m_CPUBuffer.Get(),
		descCPU.Width * descCPU.Height * descCPU.DepthOrArraySize
	);

	auto descGPU = m_GPUBuffer->GetDesc();
	MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eTextureGPU, m_GPUBuffer.Get(),
		descGPU.Width * descGPU.Height * descGPU.DepthOrArraySize
	);

	return true;
}
