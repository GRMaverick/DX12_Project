#ifndef __DeviceD3D12_h__
#define __DeviceD3D12_h__

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <map>

#include "d3dx12.h"

#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\ConstantBufferParameters.h"

class IShader;
class IBufferResource;

class CommandQueue;
class CommandList;
class SwapChain;
class CoreWindow;
class Texture2DResource;
class VertexBufferResource;
class IndexBufferResource;
class ConstantBufferResource;

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
	bool CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap* _ppDescriptorHeap, UINT _numBuffers = 1, D3D12_DESCRIPTOR_HEAP_FLAGS _flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, const wchar_t* _pDebugName = L"");
	bool CreateSwapChain(SwapChain** _ppSwapChain, CoreWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName = L"");

	Texture2DResource* CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName = L"");
	Texture2DResource* CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName = L"");
	bool CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IndexBufferResource** _ppResource, const wchar_t* _pDebugName = L"");
	bool CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, VertexBufferResource** _ppResource, const wchar_t* _pDebugName = L"");
	ConstantBufferResource* CreateConstantBufferResource(const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName = L"");
	
	bool CreateRootSignature(IShader* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName = L"");
	bool CreatePipelineState(PipelineStateDesc _psDesc, ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName = L"");
	bool CreateSamplerState(D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName = L"");

	bool FlushState();
	bool SetShader(const char* _pName);
	bool SetRenderTarget(void);
	bool SetDepthBuffer(void);
	bool SetTexture(unsigned int _iRegister, Texture2DResource* _pTexture);
	bool SetConstantBuffer(unsigned int _iRegister, ConstantBufferResource* _pCBuffer);
	bool SetSamplerState(unsigned int _iRegister, D3D12_SAMPLER_DESC _state);

	DescriptorHeap* GetSrvCbvHeap(void) { return &m_DescHeapSrvCbv; }

	void BeginFrame(void);
	void EndFrame(void);

	CommandList* GetImmediateContext(void) { return m_ImmediateContext; }
private:
	struct SamplerStateEntry
	{
		unsigned int Hash = 0;
		unsigned int HeapIndex = 0;
	};

	struct DeviceState
	{
		unsigned int			DirtyFlags = 0;
		IShader*				VertexShader = nullptr;
		IShader*				PixelShader = nullptr;
		SamplerStateEntry		Sampler[1] = { 0 };
		Texture2DResource*		Texture[1] = { 0 };
		ConstantBufferResource* ConstantBuffer[2] = { 0 };

		bool IsDirty(const unsigned int _dirtyFlags)
		{
			return (DirtyFlags & _dirtyFlags) == _dirtyFlags;
		}
	};

	const unsigned int kDirtyShaders		= 1 << 0;
	const unsigned int kDirtySamplerState	= 1 << 1;
	const unsigned int kDirtyRenderTarget	= 1 << 2;
	const unsigned int kDirtyDepthBuffer	= 1 << 3;
	const unsigned int kDirtyTexture		= 1 << 4;
	const unsigned int kDirtyConstantBuffer = 1 << 5;
	const unsigned int kDirtyRootSignature	= 1 << 6;

	DeviceD3D12(void);

	DeviceState										m_DeviceState;

	DescriptorHeap									m_DescHeapRTV;
	DescriptorHeap									m_DescHeapDSV;
	DescriptorHeap									m_DescHeapSrvCbv;
	DescriptorHeap									m_DescHeapSampler;

	DescriptorHeap									m_ActiveResourceHeap;
	DescriptorHeap									m_ActiveSamplerHeap;

	CommandList*									m_ImmediateContext;

	Microsoft::WRL::ComPtr<ID3D12Device6>			m_pDevice = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory5>			m_pDxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4>			m_pDxgiAdapter = nullptr;

	std::map<unsigned int, SamplerStateEntry>			m_mapSamplers;
	std::map<unsigned int, ID3D12RootSignature*>		m_mapRootSignatures;
	std::map<unsigned int, ID3D12PipelineState*>		m_mapPSO;
};

#endif // __DeviceD3D12_h__