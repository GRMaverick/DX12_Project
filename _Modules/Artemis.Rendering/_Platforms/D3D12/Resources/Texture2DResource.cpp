#include "Defines.h"

#include "Texture2DResource.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Resources\DescriptorHeap.h"

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "Artemis.Memory/include/MemoryGlobalTracking.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysMemory;
using namespace SysUtilities;

namespace SysRenderer
{
	namespace D3D12
	{
		Texture2DResource::Texture2DResource( const wchar_t* _pWstrFilename, const bool _bIsDds, DescriptorHeap* _pTargetSrvHeap, ID3D12Device* _pDevice, CommandList* _pCmdList, const wchar_t* _pDebugName )
		{
			m_HeapIndex = _pTargetSrvHeap->GetFreeIndexAndIncrement();

			if ( _bIsDds )
			{
				CreateFromDds( _pWstrFilename, _pDevice, _pCmdList );
			}
			else
			{
				CreateFromWic( _pWstrFilename, _pDevice, _pCmdList );
			}

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory( &srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC) );
			srvDesc.Format                        = m_gpuBuffer->GetDesc().Format;
			srvDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels           = m_gpuBuffer->GetDesc().MipLevels;
			srvDesc.Texture2D.MostDetailedMip     = 0;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			srvDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			m_hCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE( _pTargetSrvHeap->GetCpuStartHandle() );
			m_hCpuHandle.Offset( m_HeapIndex, _pTargetSrvHeap->GetIncrementSize() );
			_pDevice->CreateShaderResourceView( m_gpuBuffer.Get(), &srvDesc, m_hCpuHandle );

			wchar_t pCpuDebugName[256];
			wchar_t pGpuDebugName[256];

			wsprintf( pCpuDebugName, L"%s_%s", _pDebugName, L"CPU" );
			wsprintf( pGpuDebugName, L"%s_%s", _pDebugName, L"GPU" );

			if ( m_cpuBuffer )
				m_cpuBuffer->SetName( pCpuDebugName );

			m_gpuBuffer->SetName( pGpuDebugName );
		}

		Texture2DResource::~Texture2DResource( void )
		{
			if ( m_cpuBuffer )
			{
				MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer.Get() );
				m_cpuBuffer.Reset();
			}

			if ( m_gpuBuffer )
			{
				MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer.Get() );
				m_gpuBuffer.Reset();
			}
		}

		bool Texture2DResource::CreateFromDds( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList )
		{
			VALIDATE_D3D( CreateDDSTextureFromFile12(_pDevice, _pCmdList, _pWstrFilename, m_gpuBuffer, m_cpuBuffer) );

			const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
			MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::ETextureCpu, m_cpuBuffer.Get(), descCpu.Width * descCpu.Height * descCpu.DepthOrArraySize );

			const D3D12_RESOURCE_DESC descGpu = m_gpuBuffer->GetDesc();
			MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::ETextureGpu, m_gpuBuffer.Get(), descGpu.Width * descGpu.Height * descGpu.DepthOrArraySize );

			return true;
		}

		bool Texture2DResource::CreateFromWic( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList )
		{
			D3D12_SUBRESOURCE_DATA     srData;
			std::unique_ptr<uint8_t[]> decodedData;
			if ( FAILED( LoadWICTextureFromFile(_pDevice, _pWstrFilename, m_gpuBuffer.GetAddressOf(), decodedData, srData) ) )
				return false;

			const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
			const D3D12_RESOURCE_DESC   uploadRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( srData.SlicePitch );
			VALIDATE_D3D( _pDevice->CreateCommittedResource( &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cpuBuffer) ) );

			_pCmdList->UpdateSubresource( m_gpuBuffer.Get(), m_cpuBuffer.Get(), 0, 0, 1, &srData );

			const CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition( m_gpuBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
			_pCmdList->ResourceBarrier( 1, &tgtBarrier );

			const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
			MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::ETextureCpu, m_cpuBuffer.Get(), descCpu.Width * descCpu.Height * descCpu.DepthOrArraySize );

			const D3D12_RESOURCE_DESC descGpu = m_gpuBuffer->GetDesc();
			MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::ETextureGpu, m_gpuBuffer.Get(), descGpu.Width * descGpu.Height * descGpu.DepthOrArraySize );

			return true;
		}
	}
}
