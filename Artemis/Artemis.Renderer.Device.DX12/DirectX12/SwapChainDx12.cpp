#include "Defines.h"

#include <dxgi.h>
#include <dxgi1_6.h>
#include <assert.h>
#include <functional>

#include "Helpers\d3dx12.h"

#include "SwapChainDx12.h"
#include "CommandListDx12.h"
#include "CommandQueueDx12.h"

#include "Interfaces/IDescriptorHeap.h"

#include "Window/GameWindow.h"

#include "Memory/ScopedMemoryRecord.h"

#include "Memory/MemoryGlobalTracking.h"

using namespace Microsoft::WRL;

using namespace Artemis::Core;

namespace Artemis::Renderer::Device::Dx12
{
	SwapChainDx12::SwapChainDx12( IDXGIFactory5* _pFactory ) :
		m_pDescHeapRtv( nullptr ),
		m_pDescHeapDsv( nullptr ),
		m_vpViewport(),
		m_pFactory( _pFactory ),
		m_rectScissorRect()
	{
	}

	SwapChainDx12::~SwapChainDx12( void )
	{
		if ( m_pSwapChain )
		{
			Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_pSwapChain.Get() );
			m_pSwapChain.Reset();
		}

		if ( m_pDepthBuffer )
		{
			Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_pDepthBuffer.Get() );
			m_pDepthBuffer.Reset();
		}

		if ( m_pDxgiAdapter )
		{
			m_pDxgiAdapter.Reset();
		}

		for ( unsigned int i = 0; i < BACK_BUFFERS; ++i )
		{
			if ( m_pBackBuffers[i] )
			{
				m_pBackBuffers[i].Reset();
			}
		}
	}

	bool SwapChainDx12::Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandQueue* _pCommandQueue, unsigned int _backBuffers, Interfaces::IDescriptorHeap* _pDescHeapRTV, Interfaces::IDescriptorHeap* _pDescHeapDSV, Artemis::Core::GameWindow* _pWindow )
	{
		HRESULT hr = S_OK;

		Memory::ScopedMemoryRecord ctx( Memory::MemoryContextCategory::ERenderTarget );

		// Check Feature Support
		{
			if ( FAILED( m_pFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &m_bAllowTearing, sizeof(BOOL)) ) )
			{
				m_bAllowTearing = FALSE;
			}
		}

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);
		ID3D12CommandQueue* pQueue       = static_cast<ID3D12CommandQueue*>(_pCommandQueue->GetDeviceObject());

		// Create Swap Chain
		{
			DXGI_SWAP_CHAIN_DESC1 SwapChainDx12Desc = {};
			ZeroMemory( &SwapChainDx12Desc, sizeof(DXGI_SWAP_CHAIN_DESC1) );
			SwapChainDx12Desc.Width       = _pWindow->GetDimensions().ScreenWidth;
			SwapChainDx12Desc.Height      = _pWindow->GetDimensions().ScreenHeight;
			SwapChainDx12Desc.Format      = DXGI_FORMAT_R8G8B8A8_UNORM;
			SwapChainDx12Desc.Stereo      = FALSE;
			SwapChainDx12Desc.SampleDesc  = {1, 0};
			SwapChainDx12Desc.BufferCount = BACK_BUFFERS;
			SwapChainDx12Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			SwapChainDx12Desc.Scaling     = DXGI_SCALING_STRETCH;
			SwapChainDx12Desc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			SwapChainDx12Desc.AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED;
			SwapChainDx12Desc.Flags       = m_bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

			ComPtr<IDXGISwapChain1> pSwapChain1 = nullptr;
			hr                                  = m_pFactory->CreateSwapChainForHwnd( pQueue, _pWindow->GetWindowHandle(), &SwapChainDx12Desc, nullptr, nullptr, pSwapChain1.GetAddressOf() );

			if ( FAILED( hr ) )
			{
				assert( false && "SwapChainDx121 Creation failed" );
				return false;
			}

			hr = m_pFactory->MakeWindowAssociation( _pWindow->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER );
			if ( FAILED( hr ) )
			{
				assert( false && "MakeWindowAssociation failed" );
				return false;
			}

			hr = pSwapChain1.As( &m_pSwapChain );
			if ( FAILED( hr ) )
			{
				assert( false && "SwapChain4 cast failed" );
				return false;
			}

			constexpr unsigned int kFormatInBytes = 4; // DXGI_FORMAT_R8G8B8A8_UNORM
			Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ERenderTarget, m_pSwapChain.Get(), SwapChainDx12Desc.Width * SwapChainDx12Desc.Height * BACK_BUFFERS * kFormatInBytes );
		}

		// Create RTVs
		{
			m_pDescHeapRtv                         = _pDescHeapRTV;
			ID3D12DescriptorHeap*         pRtvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapRtv->GetDeviceObject());
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( pRtvHeap->GetCPUDescriptorHandleForHeapStart() );
			for ( UINT rtvIndex = 0; rtvIndex < BACK_BUFFERS; ++rtvIndex )
			{
				ComPtr<ID3D12Resource> pBackBuffer = nullptr;
				m_pSwapChain->GetBuffer( rtvIndex, IID_PPV_ARGS( &pBackBuffer ) );

				pDevice->CreateRenderTargetView( pBackBuffer.Get(), nullptr, rtvHandle );
				m_pBackBuffers[rtvIndex] = pBackBuffer;

				rtvHandle.Offset( m_pDescHeapRtv->GetIncrementSize() );
			}
		}
		_pWindow->AddOnResizeDelegate( std::bind( &SwapChainDx12::OnResize, this, std::placeholders::_1, std::placeholders::_2 ) );

		// Create DSV
		{
			m_pDescHeapDsv                         = _pDescHeapDSV;
			ID3D12DescriptorHeap*         pDsvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapDsv->GetDeviceObject());
			CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle( pDsvHeap->GetCPUDescriptorHandleForHeapStart() );

			D3D12_CLEAR_VALUE optimizedClearValue = {};
			ZeroMemory( &optimizedClearValue, sizeof(D3D12_CLEAR_VALUE) );
			optimizedClearValue.Format       = DXGI_FORMAT_D32_FLOAT;
			optimizedClearValue.DepthStencil = {1.0f, 0};

			D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
			D3D12_RESOURCE_DESC   defaultRdTex          = CD3DX12_RESOURCE_DESC::Tex2D( DXGI_FORMAT_D32_FLOAT, _pWindow->GetDimensions().WindowWidth, _pWindow->GetDimensions().WindowHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL );

			pDevice->CreateCommittedResource( &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &defaultRdTex, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue, IID_PPV_ARGS( m_pDepthBuffer.GetAddressOf() ) );

			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format                        = DXGI_FORMAT_D32_FLOAT;
			dsv.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D.MipSlice            = 0;
			dsv.Flags                         = D3D12_DSV_FLAG_NONE;

			pDevice->CreateDepthStencilView( m_pDepthBuffer.Get(), &dsv, dsvHandle );
			m_pDepthBuffer->SetName( L"Depth Buffer" );

			constexpr unsigned int kFormatInBytes = 4; // DXGI_FORMAT_D32_FLOAT
			Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::ERenderTarget, m_pDepthBuffer.Get(), _pWindow->GetDimensions().WindowWidth * _pWindow->GetDimensions().WindowHeight * BACK_BUFFERS * kFormatInBytes );
		}

		// Crete Viewport 
		{
			m_vpViewport      = CD3DX12_VIEWPORT( 0.0f, 0.0f, static_cast<FLOAT>(_pWindow->GetDimensions().WindowWidth), (FLOAT)_pWindow->GetDimensions().WindowHeight );
			m_rectScissorRect = CD3DX12_RECT( 0, 0, LONG_MAX, LONG_MAX );
		}
		return true;
	}

	bool SwapChainDx12::Present() const
	{
		constexpr UINT syncInterval = 0; // TODO: Support VSync
		const UINT     presentFlags = m_bAllowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0;

		return SUCCEEDED( m_pSwapChain->Present(syncInterval, presentFlags) );
	}

	void SwapChainDx12::Swap()
	{
		m_uiCurrentBackBuffer = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void SwapChainDx12::OnResize( UINT32 _width, UINT32 _height )
	{
	}

	void SwapChainDx12::PrepareForRendering( Interfaces::ICommandList* _pCmdList ) const
	{
		const CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition( m_pBackBuffers[m_uiCurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0 );
		_pCmdList->ResourceBarrier( 1, &tgtBarrier );

		ID3D12DescriptorHeap* pRtvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapRtv->GetDeviceObject());
		ID3D12DescriptorHeap* pDsvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapDsv->GetDeviceObject());

		FLOAT                               clearColour[] = {0.1f, 0.1f, 0.1f, 0.1f};
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv( pRtvHeap->GetCPUDescriptorHandleForHeapStart(), m_uiCurrentBackBuffer, m_pDescHeapRtv->GetIncrementSize() );
		const CD3DX12_CPU_DESCRIPTOR_HANDLE dsv( pDsvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_pDescHeapDsv->GetIncrementSize() );

		const CommandListDx12* pCmdList = static_cast<CommandListDx12*>(_pCmdList);
		pCmdList->ClearRenderTargetView( rtv, clearColour, 0, nullptr );
		pCmdList->ClearDepthStencilView( dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr );
		pCmdList->SetRsViewports( 1, &m_vpViewport );
		pCmdList->SetRsScissorRects( 1, &m_rectScissorRect );
	}

	void SwapChainDx12::PrepareForPresentation( Interfaces::ICommandList* _pCmdList ) const
	{
		const CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition( m_pBackBuffers[m_uiCurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0 );

		const CommandListDx12* pCmdList = static_cast<CommandListDx12*>(_pCmdList);
		pCmdList->ResourceBarrier( 1, &tgtBarrier );
	}

	void SwapChainDx12::SetOmRenderTargets( Interfaces::ICommandList* _pCmdList ) const
	{
		ID3D12DescriptorHeap*               pRtvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapRtv->GetDeviceObject());
		ID3D12DescriptorHeap*               pDsvHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapDsv->GetDeviceObject());
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv( pRtvHeap->GetCPUDescriptorHandleForHeapStart(), m_uiCurrentBackBuffer, m_pDescHeapRtv->GetIncrementSize() );
		const CD3DX12_CPU_DESCRIPTOR_HANDLE dsv( pDsvHeap->GetCPUDescriptorHandleForHeapStart(), 0, m_pDescHeapDsv->GetIncrementSize() );

		const CommandListDx12* pCmdList = static_cast<CommandListDx12*>(_pCmdList);
		pCmdList->SetOmRenderTargets( 1, &rtv, FALSE, &dsv );
	}
}
