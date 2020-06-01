#ifndef __SwapChain_h__
#define __SwapChain_h__

#include "Defines.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl.h>

class CoreWindow;
class CommandList;
class CommandQueue;
class DescriptorHeap;

class SwapChain
{
public:
	SwapChain(void);
	~SwapChain(void);

	bool Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice,
		Microsoft::WRL::ComPtr<IDXGIFactory5> _pFactory,
		CommandQueue* _pCommandQueue,
		UINT _backBuffers,
		DescriptorHeap* _pDescHeapRTV,
		DescriptorHeap* _pDescHeapDSV,
		CoreWindow* _pWindow);

	bool Present(void);
	void Swap(void);

	void PrepareForRendering(CommandList* _cmdList);
	void PrepareForPresentation(CommandList* _cmdList);

	void OnResize(UINT32 _width, UINT32 _height);

private:
	UINT											m_CurrentBackBuffer = 0;
	BOOL											m_bAllowTearing = false;

	DescriptorHeap*									m_pDescHeapRTV;
	DescriptorHeap*									m_pDescHeapDSV;

	Microsoft::WRL::ComPtr<IDXGIAdapter4>			m_pDxgiAdapter = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain4>			m_pSwapChain = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource>			m_pDepthBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>			m_pBackBuffers[BACK_BUFFERS];
};
#endif // __SwapChain_h__