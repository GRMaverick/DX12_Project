#ifndef __SwapChain_h__
#define __SwapChain_h__

//#include "Defines.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
//#include <wrl.h>

#define BACK_BUFFERS 2

namespace SysCore
{
	class GameWindow;
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
			DescriptorHeap* _pDescHeapRTV,
			DescriptorHeap* _pDescHeapDSV,
			SysCore::GameWindow* _pWindow);

		bool Present(void);
		void Swap(void);

		void PrepareForRendering(CommandList* _pCmdList);
		void PrepareForPresentation(CommandList* _pCmdList);
		void SetOMRenderTargets(CommandList* _pCmdList);

		void OnResize(UINT32 _width, UINT32 _height);

	private:
		UINT											m_CurrentBackBuffer = 0;
		BOOL											m_bAllowTearing = false;

		DescriptorHeap* m_pDescHeapRTV;
		DescriptorHeap* m_pDescHeapDSV;

		D3D12_VIEWPORT									m_Viewport;
		D3D12_RECT										m_ScissorRect;

		IDXGIAdapter4*									m_pDxgiAdapter = nullptr;
		IDXGISwapChain4*								m_pSwapChain = nullptr;

		ID3D12Resource*									m_pDepthBuffer;
		ID3D12Resource*									m_pBackBuffers[BACK_BUFFERS];
	};
}

#endif // __SwapChain_h__