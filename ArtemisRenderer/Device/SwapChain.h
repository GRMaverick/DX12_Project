#ifndef __SwapChain_h__
#define __SwapChain_h__

#include "Helpers/Defines.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>

namespace ArtemisCore::Window
{
	class ArtemisWindow;
}

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap;
}

namespace ArtemisRenderer::Device
{
	class CommandList;
	class CommandQueue;
	class DescriptorHeap;

	class SwapChain
	{
	public:
		SwapChain(void);
		~SwapChain(void);

		bool Initialise(ID3D12Device* _pDevice,
			IDXGIFactory5* _pFactory,
			CommandQueue* _pCommandQueue,
			UINT _backBuffers,
			Resources::DescriptorHeap* _pDescHeapRTV,
			Resources::DescriptorHeap* _pDescHeapDSV,
			ArtemisCore::Window::ArtemisWindow* _pWindow);

		bool Present(void);
		void Swap(void);

		void PrepareForRendering(CommandList* _pCmdList);
		void PrepareForPresentation(CommandList* _pCmdList);
		void SetOMRenderTargets(CommandList* _pCmdList);

		void OnResize(UINT32 _width, UINT32 _height);

	private:
		UINT											m_CurrentBackBuffer = 0;
		BOOL											m_bAllowTearing = false;

		Resources::DescriptorHeap*						m_pDescHeapRTV;
		Resources::DescriptorHeap*						m_pDescHeapDSV;

		D3D12_VIEWPORT									m_Viewport;
		D3D12_RECT										m_ScissorRect;

		IDXGIAdapter4*									m_pDxgiAdapter = nullptr;
		IDXGISwapChain4*								m_pSwapChain = nullptr;

		ID3D12Resource*									m_pDepthBuffer;
		ID3D12Resource*									m_pBackBuffers[BACK_BUFFERS];
	};
}

#endif // __SwapChain_h__