#include "RenderTargetResourceDx12.h"

#include <cassert>
#include <d3d12.h>

#include "CommandListDx12.h"
#include "Helpers/d3dx12.h"
#include "Memory/MemoryGlobalTracking.h"

namespace Artemis::Renderer::Device::Dx12
{
	RenderTargetResourceDx12::RenderTargetResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pRtvHeap, Interfaces::IDescriptorHeap* _pSrvHeap )
	{
		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		CD3DX12_HEAP_PROPERTIES     heapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
		const CD3DX12_RESOURCE_DESC desc           = CD3DX12_RESOURCE_DESC::Tex2D( static_cast<DXGI_FORMAT>(_format), _width, _height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET );

		const D3D12_CLEAR_VALUE clearValue = {desc.Format, {}};
		HRESULT                 hr         = pDevice->CreateCommittedResource( &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS( &m_cpuBuffer ) );
		if ( FAILED( hr ) )
		{
			assert( false && "Render Target CPU Buffer creation failed." );
		}

		const D3D12_RESOURCE_DESC descCpu = m_cpuBuffer->GetDesc();
		Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::ERenderTarget, m_cpuBuffer, GetMemoryRequirement( _width, _height, 1, desc.Format ) );

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format              = static_cast<DXGI_FORMAT>(_format);
		rtvDesc.ViewDimension       = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Buffer.FirstElement = 0;
		rtvDesc.Buffer.NumElements  = 1;

		m_RtvHeapIndex = _pRtvHeap->GetFreeIndexAndIncrement();

		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pRtvHeap->GetDeviceObject());
		m_hRtvCpuHandle             = CD3DX12_CPU_DESCRIPTOR_HANDLE( pHeap->GetCPUDescriptorHandleForHeapStart() );
		m_hRtvCpuHandle.Offset( m_RtvHeapIndex, _pRtvHeap->GetIncrementSize() );

		pDevice->CreateRenderTargetView( m_cpuBuffer, &rtvDesc, m_hRtvCpuHandle );

		if ( _pSrvHeap )
		{
			BuildShaderResourceView( pDevice, _pSrvHeap );
		}
	}

	RenderTargetResourceDx12::~RenderTargetResourceDx12( void )
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
