#ifndef __SwapChain_h__
#define __SwapChain_h__

#include "Dx12Defines.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Interfaces/ISwapChain.h"

namespace Artemis::Renderer::Interfaces
{
	class IGraphicsDevice;
	class ICommandList;
}

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Device::Dx12
{
	class CommandListDx12;
	class CommandQueueDx12;
	class DescriptorHeapDx12;

	class SwapChainDx12 final : public Interfaces::ISwapChain
	{
	public:
		SwapChainDx12( IDXGIFactory5* _pFactory );
		~SwapChainDx12( void ) override;

		bool Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandQueue* _pCommandQueue, unsigned int _backBuffers, Interfaces::IDescriptorHeap* _pDescHeapRTV, Interfaces::IDescriptorHeap* _pDescHeapDSV, Artemis::Core::GameWindow* _pWindow ) override;

		bool Present( void ) const override;
		void Swap( void ) override;

		void PrepareForRendering( Interfaces::ICommandList* _pCmdList ) const override;
		void PrepareForPresentation( Interfaces::ICommandList* _pCmdList ) const override;
		void SetOmRenderTargets( Interfaces::ICommandList* _pCmdList ) const override;

		void OnResize( UINT32 _width, UINT32 _height );

	private:
		UINT m_uiCurrentBackBuffer = 0;
		BOOL m_bAllowTearing       = false;

		Interfaces::IDescriptorHeap* m_pDescHeapRtv;
		Interfaces::IDescriptorHeap* m_pDescHeapDsv;

		D3D12_VIEWPORT m_vpViewport;
		D3D12_RECT     m_rectScissorRect;

		Microsoft::WRL::ComPtr<IDXGIAdapter4>   m_pDxgiAdapter = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_pSwapChain   = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory5>   m_pFactory     = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pBackBuffers[BACK_BUFFERS];
	};
}

#endif // __SwapChain_h__
