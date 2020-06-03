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
public:
	DeviceD3D12(void);
	~DeviceD3D12(void);

	bool Initialise(bool _bDebugging);
	bool CreateCommandQueue(D3D12_COMMAND_LIST_TYPE _type, CommandQueue** _ppCommandQueue);
	bool CreateCommandList(D3D12_COMMAND_LIST_TYPE _type, CommandList** _ppCommandList);
	bool CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap** _ppDescriptorHeap, UINT _numBuffers = 1);
	bool CreateSwapChain(SwapChain** _ppSwapChain, CoreWindow* _pWindow, UINT _numBackBuffers, CommandQueue* _pCommandQueue);
	
	bool UploadResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IBufferResource** _ppResource);
	bool CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, VertexBufferResource** _ppResource);
	bool CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IndexBufferResource** _ppResource);
	bool CreateRootSignature(D3D12_ROOT_PARAMETER1* _pRootParameters, UINT _numParameters, ID3D12RootSignature** _ppRootSignature);
	bool CreatePipelineState(PipelineStateDesc _psDesc, ID3D12PipelineState** _ppPipelineState);

private:
	Microsoft::WRL::ComPtr<ID3D12Device6>			m_pDevice = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory5>			m_pDxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4>			m_pDxgiAdapter = nullptr;
};

#endif // __DeviceD3D12_h__