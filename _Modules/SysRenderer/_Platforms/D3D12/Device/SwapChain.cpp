#include "Defines.h"

#include <dxgi.h>
#include <dxgi1_6.h>
#include <assert.h>

#include "D3D12\d3dx12.h"

#include "SwapChain.h"
#include "CommandList.h"
#include "CommandQueue.h"

#include "D3D12\Resources\DescriptorHeap.h"

#include "SysCore\_Window\GameWindow.h"

#include "SysMemory\include\ScopedMemoryRecord.h"
#include "SysMemory\include\MemoryGlobalTracking.h"

#include "SysUtilities\_Profiling\PixScopedEvent.h"

using namespace Microsoft::WRL;

using namespace SysCore;
using namespace SysMemory;

SwapChain::SwapChain(void)
{

}
SwapChain::~SwapChain(void)
{
	if (m_pSwapChain)
	{
		MemoryGlobalTracking::RecordExplicitDellocation(m_pSwapChain.Get());
		m_pSwapChain.Reset();
	}

	if (m_pDepthBuffer)
	{
		MemoryGlobalTracking::RecordExplicitDellocation(m_pDepthBuffer.Get());
		m_pDepthBuffer.Reset();
	}

	if (m_pDxgiAdapter)
	{
		m_pDxgiAdapter.Reset();
	}

	for (unsigned int i = 0; i < BACK_BUFFERS; ++i)
	{
		if (m_pBackBuffers[i])
		{
			m_pBackBuffers[i].Reset();
		}
	}
}

bool SwapChain::Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, Microsoft::WRL::ComPtr<IDXGIFactory5> _pFactory,
	CommandQueue* _pCommandQueue, UINT _backBuffers, DescriptorHeap* _pDescHeapRTV,	DescriptorHeap* _pDescHeapDSV, GameWindow* _pWindow)
{
	HRESULT hr = S_OK;

	ScopedMemoryRecord ctx(MemoryContextCategory::eRenderTarget);

	// Check Feature Support
	{
		if (FAILED(_pFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &m_bAllowTearing, sizeof(BOOL))))
		{
			m_bAllowTearing = FALSE;
		}
	}

	// Create Swap Chain
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
		swapChainDesc.Width = _pWindow->GetDimensions().ScreenWidth;
		swapChainDesc.Height = _pWindow->GetDimensions().ScreenHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = FALSE;
		swapChainDesc.SampleDesc = { 1, 0 };
		swapChainDesc.BufferCount = BACK_BUFFERS;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = m_bAllowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> pSwapChain1 = nullptr;
		hr = _pFactory->CreateSwapChainForHwnd(
			_pCommandQueue->m_pQueue.Get(),
			_pWindow->GetWindowHandle(),
			&swapChainDesc,
			nullptr,
			nullptr,
			pSwapChain1.GetAddressOf()
		);

		if (FAILED(hr))
		{
			assert(false && "SwapChain1 Creation failed");
			return false;
		}

		hr = _pFactory->MakeWindowAssociation(_pWindow->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(hr))
		{
			assert(false && "MakeWindowAssociation failed");
			return false;
		}

		hr = pSwapChain1.As(&m_pSwapChain);
		if (FAILED(hr))
		{
			assert(false && "SwapChain4 cast failed");
			return false;
		}

		const unsigned int kFormatInBytes = 4; // DXGI_FORMAT_R8G8B8A8_UNORM
		MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eRenderTarget, m_pSwapChain.Get(),
			swapChainDesc.Width * swapChainDesc.Height * BACK_BUFFERS * kFormatInBytes
		);
	}

	// Create RTVs
	{
		m_pDescHeapRTV = _pDescHeapRTV;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescHeapRTV->GetCPUStartHandle());
		for (UINT rtvIndex = 0; rtvIndex < BACK_BUFFERS; ++rtvIndex)
		{
			ComPtr<ID3D12Resource> pBackBuffer = nullptr;
			m_pSwapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&pBackBuffer));

			_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, rtvHandle);
			m_pBackBuffers[rtvIndex] = pBackBuffer;

			rtvHandle.Offset(m_pDescHeapRTV->GetIncrementSize());
		}
	}
	_pWindow->AddOnResizeDelegate(std::bind(&SwapChain::OnResize, this, std::placeholders::_1, std::placeholders::_2));

	// Create DSV
	{
		m_pDescHeapDSV = _pDescHeapDSV;
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDescHeapDSV->GetCPUStartHandle());

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		ZeroMemory(&optimizedClearValue, sizeof(D3D12_CLEAR_VALUE));
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_RESOURCE_DESC defaultRdTex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, _pWindow->GetDimensions().WindowWidth, _pWindow->GetDimensions().WindowHeight,
				1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		_pDevice->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&defaultRdTex,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(m_pDepthBuffer.GetAddressOf())
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {  };
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		_pDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), &dsv, dsvHandle);
		m_pDepthBuffer->SetName(L"Depth Buffer");

		const unsigned int kFormatInBytes = 4; // DXGI_FORMAT_D32_FLOAT
		MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory::eRenderTarget, m_pDepthBuffer.Get(),
			_pWindow->GetDimensions().WindowWidth* _pWindow->GetDimensions().WindowHeight* BACK_BUFFERS* kFormatInBytes
		);
	}

	// Crete Viewport 
	{
		m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, (FLOAT)_pWindow->GetDimensions().WindowWidth, (FLOAT)_pWindow->GetDimensions().WindowHeight);
		m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
	}
	return true;
}

bool SwapChain::Present()
{
	UINT syncInterval = 0; // TODO: Support VSync
	UINT presentFlags = m_bAllowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0;

	return SUCCEEDED(m_pSwapChain->Present(syncInterval, presentFlags));
}

void SwapChain::Swap()
{
	m_CurrentBackBuffer = m_pSwapChain->GetCurrentBackBufferIndex();
}

void SwapChain::OnResize(UINT32 _width, UINT32 _height)
{

}

void SwapChain::PrepareForRendering(CommandList* _pCmdList)
{
	CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pBackBuffers[m_CurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0);
	_pCmdList->ResourceBarrier(1, &tgtBarrier);

	FLOAT clearColour[] = { 0.1f, 0.1f, 0.1f, 0.1f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pDescHeapRTV->GetCPUStartHandle(), m_CurrentBackBuffer, m_pDescHeapRTV->GetIncrementSize());
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(m_pDescHeapDSV->GetCPUStartHandle(), 0, m_pDescHeapDSV->GetIncrementSize());

	_pCmdList->ClearRenderTargetView(rtv, clearColour, 0, nullptr);
	_pCmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	_pCmdList->SetRSViewports(1, &m_Viewport);
	_pCmdList->SetRSScissorRects(1, &m_ScissorRect);
}

void SwapChain::PrepareForPresentation(CommandList* _pCmdList)
{
	CD3DX12_RESOURCE_BARRIER tgtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pBackBuffers[m_CurrentBackBuffer].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0);
	_pCmdList->ResourceBarrier(1, &tgtBarrier);
}

void SwapChain::SetOMRenderTargets(CommandList* _pCmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pDescHeapRTV->GetCPUStartHandle(), m_CurrentBackBuffer, m_pDescHeapRTV->GetIncrementSize());
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(m_pDescHeapDSV->GetCPUStartHandle(), 0, m_pDescHeapDSV->GetIncrementSize());

	_pCmdList->SetOMRenderTargets(1, &rtv, FALSE, &dsv);
}