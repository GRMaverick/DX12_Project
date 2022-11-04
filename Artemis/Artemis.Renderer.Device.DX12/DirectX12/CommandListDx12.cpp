#include "Dx12Defines.h"

#include "CommandListDx12.h"

#include "VertexBufferResourceDx12.h"
#include "IndexBufferResourceDx12.h"

#include "pix3.h"

#include <assert.h>

#include <ImGUI\imgui_impl_dx12.h>

//#pragma comment(lib, "D:\\Training\\DX12_Project\\packages\\WinPixEventRuntime.1.0.220810001\\bin\\x64\\WinPixEventRuntime.lib")
#pragma comment(lib, "C:\\Users\\grmav\\source\\repos\\DX12_Project\\packages\\WinPixEventRuntime.1.0.220810001\\bin\\x64\\WinPixEventRuntime.lib")

using namespace Microsoft::WRL;

const char* g_TypeToString[]{"GFX List", "Bundle List", "Compute List", "Copy List", "Video Decode", "Video Process", "Video Encode",};

namespace Artemis::Renderer::Device::Dx12
{
	CommandListDx12::CommandListDx12( void ) :
		m_bIsRecording( false ),
		m_pName{},
		m_clType()
	{
		m_pList      = nullptr;
		m_pAllocator = nullptr;
	}

	CommandListDx12::~CommandListDx12( void )
	{
		if ( m_pList )
			m_pList.Reset();
		if ( m_pAllocator )
			m_pAllocator.Reset();
	}

	bool CommandListDx12::Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ECommandListType _type, const wchar_t* _pDebugName )
	{
		HRESULT hr     = S_OK;
		m_clType       = static_cast<D3D12_COMMAND_LIST_TYPE>(_type);
		m_bIsRecording = false;

		size_t szConverted = 0;
		wcstombs_s( &szConverted, m_pName, _pDebugName, ARRAYSIZE( m_pName ) );

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		// Command Allocator
		{
			hr = pDevice->CreateCommandAllocator( m_clType, IID_PPV_ARGS( m_pAllocator.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Command Allocator Creation Failed" );
				hr = pDevice->GetDeviceRemovedReason();
				if ( FAILED( hr ) )
				{
					assert( false );
				}
				return false;
			}
			m_pAllocator->SetName( _pDebugName );
		}

		// Command List
		{
			hr = pDevice->CreateCommandList( 0, m_clType, m_pAllocator.Get(), nullptr, IID_PPV_ARGS( m_pList.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Command List Creation Failed" );
				return false;
			}
			m_pList->SetName( _pDebugName );
		}

#if defined(_DEBUG) && defined(BREADCRUMB)
		// Init Breadcrumb Debugging
		{
			auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
			auto buff = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT32));
			VALIDATE_D3D(_pDevice->CreateCommittedResource(
				&props,
				D3D12_HEAP_FLAG_NONE,
				&buff,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_BreadcrumbBuffer)));

			m_BreadcrumbBuffer->Map(0, nullptr, (void**)&m_BreadcrumbReadback);
		}
#endif

		Close();

		return true;
	}

	void CommandListDx12::Close( void ) const
	{
		m_pList->Close();
	}

	void CommandListDx12::Reset( void ) const
	{
		m_pAllocator->Reset();
		m_pList->Reset( m_pAllocator.Get(), nullptr );
	}

	void CommandListDx12::StartMarker( const char* _pFormatString ) const
	{
		// CPU & GPU
		PIXBeginEvent( m_pList.Get(), PIX_COLOR_INDEX( 0 ), _pFormatString );
	}

	void CommandListDx12::EndMarker( void ) const
	{
		// CPU & GPU
		PIXEndEvent( m_pList.Get() );
	}

	void CommandListDx12::WriteBreadcrumb( UINT32 _breadcrumb )
	{
#if defined(_DEBUG) && defined(BREADCRUMB)
		ComPtr<ID3D12GraphicsCommandList2> pList2 = nullptr;
		VALIDATE_D3D(m_pList.As(&pList2));

		D3D12_WRITEBUFFERIMMEDIATE_MODE modes = D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT;
		D3D12_WRITEBUFFERIMMEDIATE_PARAMETER params;
		params.Dest = m_BreadcrumbBuffer->GetGPUVirtualAddress();
		params.Value = _breadcrumb;
		pList2->WriteBufferImmediate(1, &params, &modes);
#endif
	}

	UINT32 CommandListDx12::ReadBreadcrumb( void )
	{
#if defined(_DEBUG) && defined(BREADCRUMB)
		return *m_BreadcrumbReadback;
#else
		return 0;
#endif
	}

	void CommandListDx12::ResourceBarrier( const UINT32 _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "ResourceBarrier" );
		m_pList->ResourceBarrier( _numBarriers, _pBarrier );
	}

	void CommandListDx12::UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const UINT _intermediateOffset, const UINT _firstSubresource, const UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "UpdateSubresource" );
		UpdateSubresources( m_pList.Get(), _pGpu, _pCpu, _intermediateOffset, _firstSubresource, _numSubresources, _pSubresourceData );
	}

	void CommandListDx12::ClearRenderTargetView( D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], const UINT _numRects, const D3D12_RECT* _pRects ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "ClearRenderTargetView" );
		m_pList->ClearRenderTargetView( _cpuHandle, _pColor, _numRects, _pRects );
	}

	void CommandListDx12::ClearDepthStencilView( const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const D3D12_CLEAR_FLAGS _clearFlags, const FLOAT _depth, const UINT8 _stencil, const UINT _numRects, const D3D12_RECT* _pRects ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "ClearDepthStencilView" );
		m_pList->ClearDepthStencilView( _cpuHandle, _clearFlags, _depth, _stencil, _numRects, _pRects );
	}

	void CommandListDx12::SetPipelineState( ID3D12PipelineState* _pPipelineState ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetPipelineState" );
		m_pList->SetPipelineState( _pPipelineState );
	}

	void CommandListDx12::SetGraphicsRootSignature( ID3D12RootSignature* _pRootSignature ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetGraphicsRootSignature" );
		m_pList->SetGraphicsRootSignature( _pRootSignature );
	}

	void CommandListDx12::SetGraphicsRootConstantBufferView( const UINT _rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS _gpuAddress ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetGraphicsRootConstantBufferView" );
		m_pList->SetGraphicsRootConstantBufferView( _rootParameterIndex, _gpuAddress );
	}

	void CommandListDx12::SetGraphicsRootDescriptorTable( const UINT _rootParameterIndex, const D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetGraphicsRootDescriptorTable" );
		m_pList->SetGraphicsRootDescriptorTable( _rootParameterIndex, _gpuHandle );
	}

	void CommandListDx12::SetGraphicsRoot32BitConstants( const UINT _rootParameterIndex, const UINT _num32BitValuesToSet, const void* _pSrcData, const UINT _destOffsetIn32BitValues ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetGraphicsRoot32BitConstants" );
		m_pList->SetGraphicsRoot32BitConstants( _rootParameterIndex, _num32BitValuesToSet, _pSrcData, _destOffsetIn32BitValues );
	}

	void CommandListDx12::SetRsViewports( UINT _numViewports, const D3D12_VIEWPORT* _pViewport ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetRSViewports" );
		m_pList->RSSetViewports( _numViewports, _pViewport );
	}

	void CommandListDx12::SetRsScissorRects( const UINT _numRects, const D3D12_RECT* _pScissorRects ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetRSScissorRects" );
		m_pList->RSSetScissorRects( _numRects, _pScissorRects );
	}

	void CommandListDx12::SetIaPrimitiveTopology( Interfaces::PrimitiveTopology _topology ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetIAPrimitiveTopology" );
		m_pList->IASetPrimitiveTopology( static_cast<D3D12_PRIMITIVE_TOPOLOGY>(_topology) );
	}

	void CommandListDx12::SetIaVertexBuffers( const unsigned int _startSlot, unsigned int _numViews, void* _pViews ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetIAVertexBuffers" );
		m_pList->IASetVertexBuffers( _startSlot, _numViews, static_cast<D3D12_VERTEX_BUFFER_VIEW*>(_pViews) );
	}

	void CommandListDx12::SetIaIndexBuffer( void* _pView ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetIAIndexBuffer" );
		m_pList->IASetIndexBuffer( static_cast<D3D12_INDEX_BUFFER_VIEW*>(_pView) );
	}

	void CommandListDx12::SetOmRenderTargets( const UINT _numRTs, const D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, const BOOL _bSingleHandleToDescriptor, const D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetOMRenderTargets" );
		m_pList->OMSetRenderTargets( _numRTs, _rtCpuDescHandle, _bSingleHandleToDescriptor, _dsvCpuDescHandle );
	}

	void CommandListDx12::DrawImGui() const
	{
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), m_pList.Get() );
	}

	void CommandListDx12::DrawIndexedInstanced( Interfaces::IGpuResource* _pVertexBuffer, Interfaces::IGpuResource* _pIndexBuffer, unsigned int _indices ) const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = static_cast<VertexBufferResourceDx12*>(_pVertexBuffer)->GetView();
		D3D12_INDEX_BUFFER_VIEW  ibv = static_cast<IndexBufferResourceDx12*>(_pIndexBuffer)->GetView();

		SetIaVertexBuffers( 0, 1, &vbv );
		SetIaIndexBuffer( &ibv );
		DrawIndexedInstanced( _indices, 1, 0, 0, 0 );
	}

	void CommandListDx12::DrawIndexedInstanced( const unsigned int _indicesPerInstance, const unsigned int _instanceCount, const unsigned int _startIndexLocation, const unsigned int _baseVertexLocation, const unsigned int _startInstanceLocation ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "DrawIndexedInstanced" );
		m_pList->DrawIndexedInstanced( _indicesPerInstance, _instanceCount, _startIndexLocation, _baseVertexLocation, _startInstanceLocation );
	}

	void CommandListDx12::SetDescriptorHeaps( Interfaces::IDescriptorHeap** _ppHeaps, const unsigned int _uiHeaps ) const
	{
		LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_clType], "SetDescriptorHeaps" );
		std::vector<ID3D12DescriptorHeap*> vpHeaps;
		vpHeaps.resize( _uiHeaps );

		for ( unsigned int i = 0; i < _uiHeaps; ++i )
		{
			vpHeaps[i] = static_cast<ID3D12DescriptorHeap*>(_ppHeaps[i]->GetDeviceObject());
		}

		m_pList->SetDescriptorHeaps( static_cast<UINT>(vpHeaps.size()), &vpHeaps[0] );
	}
}
