#include "Defines.h"

#include "CommandList.h"

#include <assert.h>

#include <ImGUI\imgui_impl_dx12.h>

using namespace Microsoft::WRL;

const char* g_TypeToString[]{"GFX List", "Bundle List", "Compute List", "Copy List", "Video Decode", "Video Process", "Video Encode",};

namespace SysRenderer
{
	namespace D3D12
	{
		CommandList::CommandList( void ):
			m_bIsRecording( false ),
			m_pName{},
			m_clType()
		{
			m_pList      = nullptr;
			m_pAllocator = nullptr;
		}

		CommandList::~CommandList( void )
		{
			if ( m_pList )
				m_pList.Reset();
			if ( m_pAllocator )
				m_pAllocator.Reset();
		}

		bool CommandList::Initialise( ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName )
		{
			HRESULT hr     = S_OK;
			m_clType       = _type;
			m_bIsRecording = false;

			size_t szConverted = 0;
			wcstombs_s( &szConverted, m_pName, _pDebugName, ARRAYSIZE( m_pName ) );

			// Command Allocator
			{
				hr = _pDevice->CreateCommandAllocator( _type, IID_PPV_ARGS( m_pAllocator.GetAddressOf() ) );
				if ( FAILED( hr ) )
				{
					assert( false && "Command Allocator Creation Failed" );
					hr = _pDevice->GetDeviceRemovedReason();
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
				hr = _pDevice->CreateCommandList( 0, _type, m_pAllocator.Get(), nullptr, IID_PPV_ARGS( m_pList.GetAddressOf() ) );
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

		void CommandList::Close( void ) const
		{
			m_pList->Close();
		}

		void CommandList::Reset( void ) const
		{
			m_pAllocator->Reset();
			m_pList->Reset( m_pAllocator.Get(), nullptr );
		}

		void CommandList::WriteBreadcrumb( UINT32 _breadcrumb )
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

		UINT32 CommandList::ReadBreadcrumb( void )
		{
#if defined(_DEBUG) && defined(BREADCRUMB)
			return *m_BreadcrumbReadback;
#else
			return 0;
#endif
		}

		void CommandList::ResourceBarrier( const UINT32 _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "ResourceBarrier" );
			m_pList->ResourceBarrier( _numBarriers, _pBarrier );
		}

		void CommandList::UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const UINT _intermediateOffset, const UINT _firstSubresource, const UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "UpdateSubresource" );
			UpdateSubresources( m_pList.Get(), _pGpu, _pCpu, _intermediateOffset, _firstSubresource, _numSubresources, _pSubresourceData );
		}

		void CommandList::ClearRenderTargetView( D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], const UINT _numRects, const D3D12_RECT* _pRects ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "ClearRenderTargetView" );
			m_pList->ClearRenderTargetView( _cpuHandle, _pColor, _numRects, _pRects );
		}

		void CommandList::ClearDepthStencilView( const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const D3D12_CLEAR_FLAGS _clearFlags, const FLOAT _depth, const UINT8 _stencil, const UINT _numRects, const D3D12_RECT* _pRects ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "ClearDepthStencilView" );
			m_pList->ClearDepthStencilView( _cpuHandle, _clearFlags, _depth, _stencil, _numRects, _pRects );
		}

		void CommandList::SetPipelineState( ID3D12PipelineState* _pPipelineState ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetPipelineState" );
			m_pList->SetPipelineState( _pPipelineState );
		}

		void CommandList::SetGraphicsRootSignature( ID3D12RootSignature* _pRootSignature ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetGraphicsRootSignature" );
			m_pList->SetGraphicsRootSignature( _pRootSignature );
		}

		void CommandList::SetGraphicsRootConstantBufferView( const UINT _rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS _gpuAddress ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetGraphicsRootConstantBufferView" );
			m_pList->SetGraphicsRootConstantBufferView( _rootParameterIndex, _gpuAddress );
		}

		void CommandList::SetGraphicsRootDescriptorTable( const UINT _rootParameterIndex, const D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetGraphicsRootDescriptorTable" );
			m_pList->SetGraphicsRootDescriptorTable( _rootParameterIndex, _gpuHandle );
		}

		void CommandList::SetGraphicsRoot32BitConstants( const UINT _rootParameterIndex, const UINT _num32BitValuesToSet, const void* _pSrcData, const UINT _destOffsetIn32BitValues ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetGraphicsRoot32BitConstants" );
			m_pList->SetGraphicsRoot32BitConstants( _rootParameterIndex, _num32BitValuesToSet, _pSrcData, _destOffsetIn32BitValues );
		}

		void CommandList::SetRsViewports( UINT _numViewports, const D3D12_VIEWPORT* _pViewport ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetRSViewports" );
			m_pList->RSSetViewports( _numViewports, _pViewport );
		}

		void CommandList::SetRsScissorRects( const UINT _numRects, const D3D12_RECT* _pScissorRects ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetRSScissorRects" );
			m_pList->RSSetScissorRects( _numRects, _pScissorRects );
		}

		void CommandList::SetIaPrimitiveTopology( const D3D12_PRIMITIVE_TOPOLOGY _topology ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetIAPrimitiveTopology" );
			m_pList->IASetPrimitiveTopology( _topology );
		}

		void CommandList::SetIaVertexBuffers( const UINT _startSlot, UINT _numViews, const D3D12_VERTEX_BUFFER_VIEW* _pViews ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetIAVertexBuffers" );
			m_pList->IASetVertexBuffers( _startSlot, _numViews, _pViews );
		}

		void CommandList::SetIaIndexBuffer( const D3D12_INDEX_BUFFER_VIEW* _pView ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetIAIndexBuffer" );
			m_pList->IASetIndexBuffer( _pView );
		}

		void CommandList::SetOmRenderTargets( const UINT _numRTs, const D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, const BOOL _bSingleHandleToDescriptor, const D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetOMRenderTargets" );
			m_pList->OMSetRenderTargets( _numRTs, _rtCpuDescHandle, _bSingleHandleToDescriptor, _dsvCpuDescHandle );
		}

		void CommandList::DrawImGui() const
		{
			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData( ImGui::GetDrawData(), m_pList.Get() );
		}

		void CommandList::DrawIndexedInstanced( const UINT _indicesPerInstance, const UINT _instanceCount, const UINT _startIndexLocation, const UINT _baseVertexLocation, const UINT _startInstanceLocation ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "DrawIndexedInstanced" );
			m_pList->DrawIndexedInstanced( _indicesPerInstance, _instanceCount, _startIndexLocation, _baseVertexLocation, _startInstanceLocation );
		}

		void CommandList::SetDescriptorHeaps( ID3D12DescriptorHeap* const* _pHeaps, const UINT _numHeaps ) const
		{
			LOW_LEVEL_PROFILE_MARKER( this, "%s: %s", g_TypeToString[m_Type], "SetDescriptorHeaps" );
			m_pList->SetDescriptorHeaps( _numHeaps, _pHeaps );
		}
	}
}
