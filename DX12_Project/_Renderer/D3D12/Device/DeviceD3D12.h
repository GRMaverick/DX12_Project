#ifndef __DeviceD3D12_h__
#define __DeviceD3D12_h__

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "d3dx12.h"

class CommandQueue;
class CommandList;
class SwapChain;
class DescriptorHeap;
class CoreWindow;
class IBufferResource;
class Texture2DResource;
class VertexBufferResource;
class IndexBufferResource;

struct PipelineStateDesc
{
	CD3DX12_PIPELINE_STATE_STREAM_VS VertexShader;
	CD3DX12_PIPELINE_STATE_STREAM_PS PixelShader;
	CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
	CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
};

class DeviceD3D12
{
	friend class RendererD3D12;
public:
	~DeviceD3D12(void);

	static DeviceD3D12* Instance(void);

	bool Initialise(bool _bDebugging);
	bool InitialiseImGUI(HWND _hWindow, DescriptorHeap* _pSRVHeap);

	bool CreateCommandQueue(D3D12_COMMAND_LIST_TYPE _type, CommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"");
	bool CreateCommandList(D3D12_COMMAND_LIST_TYPE _type, CommandList** _ppCommandList, const wchar_t* _pDebugName = L"");
	bool CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap** _ppDescriptorHeap, UINT _numBuffers = 1, D3D12_DESCRIPTOR_HEAP_FLAGS _flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, const wchar_t* _pDebugName = L"");
	bool CreateSwapChain(SwapChain** _ppSwapChain, CoreWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName = L"");

	bool CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, DescriptorHeap* _pDescHeapSRV, const wchar_t* _pDebugName = L"");
	bool CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, DescriptorHeap* _pDescHeapSRV, const wchar_t* _pDebugName = L"");
	bool CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IndexBufferResource** _ppResource, const wchar_t* _pDebugName = L"");
	bool CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, VertexBufferResource** _ppResource, const wchar_t* _pDebugName = L"");

	bool CreateRootSignature(D3D12_ROOT_PARAMETER* _pRootParameters, UINT _numParameters, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName = L"");
	bool CreatePipelineState(PipelineStateDesc _psDesc, ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName = L"");
	bool CreateSamplerState(D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName = L"");

private:
	DeviceD3D12(void);
	Microsoft::WRL::ComPtr<ID3D12Device6>			m_pDevice = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory5>			m_pDxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4>			m_pDxgiAdapter = nullptr;
};

#endif // __DeviceD3D12_h__