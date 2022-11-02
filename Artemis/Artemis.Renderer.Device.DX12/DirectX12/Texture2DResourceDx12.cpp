#include "Dx12Defines.h"

#include "Texture2DResourceDx12.h"

#include "DeviceDx12.h"
#include "CommandListDx12.h"
#include "DescriptorHeapDx12.h"

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "Memory/MemoryGlobalTracking.h"

using namespace DirectX;
using namespace Artemis::Utilities;

namespace Artemis::Renderer::Device::Dx12
{
	Texture2DResourceDx12::Texture2DResourceDx12( const wchar_t* _pWstrFilename, const bool _bIsDds, const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCmdList, Interfaces::IDescriptorHeap* _pTargetSrvHeap, const wchar_t* _pDebugName )
	{
		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		if ( _bIsDds )
		{
			CreateFromDds( _pWstrFilename, pDevice, static_cast<CommandListDx12*>(_pCmdList) );
		}
		else
		{
			CreateFromWic( _pWstrFilename, pDevice, static_cast<CommandListDx12*>(_pCmdList) );
		}

		m_SrvHeapIndex = _pTargetSrvHeap->GetFreeIndexAndIncrement();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory( &srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC) );
		srvDesc.Format                        = m_gpuBuffer->GetDesc().Format;
		srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels           = m_gpuBuffer->GetDesc().MipLevels;
		srvDesc.Texture2D.MostDetailedMip     = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pTargetSrvHeap->GetDeviceObject());
		m_hSrvCpuHandle             = CD3DX12_CPU_DESCRIPTOR_HANDLE( pHeap->GetCPUDescriptorHandleForHeapStart() );
		m_hSrvCpuHandle.Offset( m_SrvHeapIndex, _pTargetSrvHeap->GetIncrementSize() );
		pDevice->CreateShaderResourceView( m_gpuBuffer, &srvDesc, m_hSrvCpuHandle );

		wchar_t pCpuDebugName[256];
		wchar_t pGpuDebugName[256];

		wsprintf( pCpuDebugName, L"%s_%s", _pDebugName, L"CPU" );
		wsprintf( pGpuDebugName, L"%s_%s", _pDebugName, L"GPU" );

		if ( m_cpuBuffer )
			m_cpuBuffer->SetName( pCpuDebugName );

		m_gpuBuffer->SetName( pGpuDebugName );
	}

	Texture2DResourceDx12::~Texture2DResourceDx12( void )
	{
		if ( m_cpuBuffer )
		{
			Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer );
			m_cpuBuffer->Release();
			delete m_cpuBuffer;
		}

		if ( m_gpuBuffer )
		{
			Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer );
			m_gpuBuffer->Release();
			delete m_gpuBuffer;
		}
	}

	bool Texture2DResourceDx12::CreateFromDds( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList )
	{
		VALIDATE_D3D( CreateDDSTextureFromFile12( _pDevice, _pCmdList, _pWstrFilename, m_gpuBuffer, m_cpuBuffer ) );

		const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
		Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ETextureCpu, m_cpuBuffer, descCpu.Width * descCpu.Height * descCpu.DepthOrArraySize );

		const D3D12_RESOURCE_DESC descGpu = m_gpuBuffer->GetDesc();
		Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ETextureGpu, m_gpuBuffer, descGpu.Width * descGpu.Height * descGpu.DepthOrArraySize );

		return true;
	}

	bool Texture2DResourceDx12::CreateFromWic( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList )
	{
		D3D12_SUBRESOURCE_DATA     srData;
		std::unique_ptr<uint8_t[]> decodedData;
		if ( FAILED( LoadWICTextureFromFile(_pDevice, _pWstrFilename, &m_gpuBuffer, decodedData, srData) ) )
			return false;

		const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
		const D3D12_RESOURCE_DESC   uploadRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( srData.SlicePitch );
		VALIDATE_D3D( _pDevice->CreateCommittedResource( &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_cpuBuffer ) ) );

		_pCmdList->UpdateSubresource( m_gpuBuffer, m_cpuBuffer, 0, 0, 1, &srData );

		const CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition( m_gpuBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
		_pCmdList->ResourceBarrier( 1, &tgtBarrier );

		const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
		Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ETextureCpu, m_cpuBuffer, descCpu.Width * descCpu.Height * descCpu.DepthOrArraySize );

		const D3D12_RESOURCE_DESC descGpu = m_gpuBuffer->GetDesc();
		Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ETextureGpu, m_gpuBuffer, descGpu.Width * descGpu.Height * descGpu.DepthOrArraySize );

		return true;
	}
}
