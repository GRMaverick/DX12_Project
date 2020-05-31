#include "Defines.h"

#include <dxgi.h>
#include <dxgi1_6.h>
#include <assert.h>
#include "d3dx12.h"

#include "SwapChain.h"
#include "CoreWindow.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "DescriptorHeap.h"

using namespace Microsoft::WRL;

SwapChain::SwapChain(void)
{

}
SwapChain::~SwapChain(void)
{

}

bool SwapChain::Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, Microsoft::WRL::ComPtr<IDXGIFactory5> _pFactory,
	CommandQueue* _pCommandQueue, UINT _backBuffers, DescriptorHeap* _pDescHeapRTV,	CoreWindow* _pWindow)
{
	HRESULT hr = S_OK;

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
	}

	// Create RTVs
	{
		m_pDescHeapRTV = _pDescHeapRTV;
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescHeapRTV->GetCPUStartHandle());
		for (UINT rtvIndex = 0; rtvIndex < BACK_BUFFERS; ++rtvIndex)
		{
			ComPtr<ID3D12Resource> pBackBuffer;
			m_pSwapChain->GetBuffer(rtvIndex, IID_PPV_ARGS(&pBackBuffer));

			_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, rtvHandle);
			m_BackBuffers[rtvIndex] = pBackBuffer;

			rtvHandle.Offset(m_pDescHeapRTV->GetIncrementSize());
		}
	}
	_pWindow->AddOnResizeDelegate(std::bind(&SwapChain::OnResize, this, std::placeholders::_1, std::placeholders::_2));

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

void SwapChain::PrepareForRendering(CommandList* _cmdList)
{
	_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BackBuffers[m_CurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, 0));

	FLOAT clearColour[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_pDescHeapRTV->GetCPUStartHandle(), m_CurrentBackBuffer, m_pDescHeapRTV->GetIncrementSize());

	_cmdList->ClearRenderTargetView(rtv, clearColour, 0, nullptr);
}
void SwapChain::PrepareForPresentation(CommandList* _cmdList)
{
	_cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_BackBuffers[m_CurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, 0));
}