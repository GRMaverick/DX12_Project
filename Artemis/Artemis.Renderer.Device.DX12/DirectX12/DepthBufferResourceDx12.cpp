#include "DepthBufferResourceDx12.h"

#include <cassert>
#include <d3d12.h>

#include "CommandListDx12.h"
#include "Helpers/d3dx12.h"
#include "Memory/MemoryGlobalTracking.h"

namespace Artemis::Renderer::Device::Dx12
{
	DepthBufferResourceDx12::DepthBufferResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pDsvHeap, Interfaces::IDescriptorHeap* _pSrvHeap )
	{
		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		const CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );

		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory( &texDesc, sizeof(D3D12_RESOURCE_DESC) );
		texDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment          = 0;
		texDesc.Width              = _width;
		texDesc.Height             = _height;
		texDesc.DepthOrArraySize   = 1;
		texDesc.MipLevels          = 1;
		texDesc.Format             = static_cast<DXGI_FORMAT>(_format);
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth   = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		const HRESULT hr = pDevice->CreateCommittedResource( &heapProperties, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS( &m_cpuBuffer ) );
		if ( FAILED( hr ) )
		{
			assert( false && "Render Target CPU Buffer creation failed." );
		}

		const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
		Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::ERenderTarget, m_cpuBuffer, GetMemoryRequirement( _width, _height, 1, texDesc.Format ) );

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags              = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;

		m_DsvHeapIndex = _pDsvHeap->GetFreeIndexAndIncrement();

		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pDsvHeap->GetDeviceObject());
		m_hDsvCpuHandle             = CD3DX12_CPU_DESCRIPTOR_HANDLE( pHeap->GetCPUDescriptorHandleForHeapStart() );
		m_hDsvCpuHandle.Offset( m_DsvHeapIndex, _pDsvHeap->GetIncrementSize() );

		pDevice->CreateDepthStencilView( m_cpuBuffer, &dsvDesc, m_hDsvCpuHandle );

		if ( _pSrvHeap )
		{
			BuildShaderResourceView( pDevice, _pSrvHeap );
		}
	}

	DepthBufferResourceDx12::~DepthBufferResourceDx12( void )
	{
		if ( m_cpuBuffer )
		{
			Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer );
			m_cpuBuffer->Release();
			delete m_cpuBuffer;
		}

		if ( m_gpuBuffer )
		{
			Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer );
			m_gpuBuffer->Release();
			delete m_gpuBuffer;
		}
	}
}
