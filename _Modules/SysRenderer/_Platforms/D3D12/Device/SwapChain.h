#ifndef __SwapChain_h__
#define __SwapChain_h__

#include "Defines.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace SysCore
{
	class GameWindow;
}

namespace SysRenderer
{
	namespace D3D12
	{
		class CommandList;
		class CommandQueue;
		class DescriptorHeap;

		class SwapChain
		{
		public:
			SwapChain( void );
			~SwapChain( void );

			bool Initialise( Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, Microsoft::WRL::ComPtr<IDXGIFactory5> _pFactory, CommandQueue* _pCommandQueue, UINT _backBuffers, DescriptorHeap* _pDescHeapRTV, DescriptorHeap* _pDescHeapDSV, SysCore::GameWindow* _pWindow );

			bool Present( void ) const;
			void Swap( void );

			void PrepareForRendering( const CommandList* _pCmdList ) const;
			void PrepareForPresentation( const CommandList* _pCmdList ) const;
			void SetOmRenderTargets( const CommandList* _pCmdList ) const;

			void OnResize( UINT32 _width, UINT32 _height );

		private:
			UINT m_uiCurrentBackBuffer = 0;
			BOOL m_bAllowTearing       = false;

			DescriptorHeap* m_pDescHeapRtv;
			DescriptorHeap* m_pDescHeapDsv;

			D3D12_VIEWPORT m_vpViewport;
			D3D12_RECT     m_rectScissorRect;

			Microsoft::WRL::ComPtr<IDXGIAdapter4>   m_pDxgiAdapter = nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain   = nullptr;

			Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthBuffer;
			Microsoft::WRL::ComPtr<ID3D12Resource> m_pBackBuffers[BACK_BUFFERS];
		};
	}
}

#endif // __SwapChain_h__
