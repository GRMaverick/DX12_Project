#include "Defines.h"

#include "DeviceD3D12.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "CoreWindow.h"

#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\Texture2DResource.h"
#include "D3D12\Resources\VertexBufferResource.h"
#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\ConstantBufferResource.h"
#include "D3D12\Resources\UploadBuffer.h"

#include <assert.h>
#include <DirectXMath.h>

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "SysMemory/include/ScopedMemoryRecord.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysMemory;

DeviceD3D12* DeviceD3D12::Instance(void)
{
	static DeviceD3D12 device;
	return &device;
}

DeviceD3D12::DeviceD3D12(void)
{
	m_pDevice = nullptr;
	m_pDxgiFactory = nullptr;
	m_pDxgiAdapter = nullptr;
}

DeviceD3D12::~DeviceD3D12(void)
{
	if (m_pDevice) m_pDevice.Reset();
	if (m_pDxgiFactory) m_pDxgiFactory.Reset();
	if (m_pDxgiAdapter) m_pDxgiAdapter.Reset();
}

bool DeviceD3D12::Initialise(bool _bDebugging)
{
	// Query Adapter
	HRESULT hr = S_OK;
	{
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_pDxgiFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "DXGI Factory Creation Failed");
			return false;
		}

		SIZE_T maxDedicatedVMEM = 0;
		ComPtr<IDXGIAdapter1> pAdapter1 = nullptr;
		for (UINT adapter = 0; m_pDxgiFactory->EnumAdapters1(adapter, pAdapter1.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++adapter)
		{
			DXGI_ADAPTER_DESC1 adapterDesc1 = {};
			ZeroMemory(&adapterDesc1, sizeof(DXGI_ADAPTER_DESC1));
			pAdapter1->GetDesc1(&adapterDesc1);

			bool bIsSoftwareFlag = (adapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
			bool bCanCreateDevice = SUCCEEDED(D3D12CreateDevice(pAdapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr));
			bool bHasMoreVMEM = adapterDesc1.DedicatedVideoMemory > maxDedicatedVMEM;
			if (bIsSoftwareFlag && bCanCreateDevice && bHasMoreVMEM)
			{
				maxDedicatedVMEM = adapterDesc1.DedicatedVideoMemory;
				hr = pAdapter1.As(&m_pDxgiAdapter);
				if (FAILED(hr))
				{
					assert(false && "Found appropriate adapter but failed casting to IDXGIAdapter4");
					return false;
				}
			}
		}
	}

	// If Debugging Enabled
	if (_bDebugging)
	{
		ComPtr<ID3D12Debug> spDebugController0;
		ComPtr<ID3D12Debug1> spDebugController1;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));
		if (FAILED(hr))
		{
			assert(false && "No Debug Interface");
		}

		hr = spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
		if (FAILED(hr))
		{
			assert(false && "No Debug 1 Interface");
		}

		spDebugController1->EnableDebugLayer();

		ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings));
		if (SUCCEEDED(hr))
		{
			pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}
		spDebugController1->SetEnableGPUBasedValidation(true);
	}

	// --------------------------------------------------------------------------------------------------------------------------------
	// Experimental Feature: D3D12ExperimentalShaderModels
	//
	// Use with D3D12EnableExperimentalFeatures to enable experimental shader model support,
	// meaning shader models that haven't been finalized for use in retail.
	//
	// Enabling D3D12ExperimentalShaderModels needs no configuration struct, pass NULL in the pConfigurationStructs array.
	//
	// --------------------------------------------------------------------------------------------------------------------------------
	static const UUID D3D12ExperimentalShaderModels = { /* 76f5573e-f13a-40f5-b297-81ce9e18933f */
		0x76f5573e,
		0xf13a,
		0x40f5,
		{ 0xb2, 0x97, 0x81, 0xce, 0x9e, 0x18, 0x93, 0x3f }
	};

	VALIDATE_D3D(D3D12EnableExperimentalFeatures(1, &D3D12ExperimentalShaderModels, nullptr, nullptr));
	
	// Create Device
	VALIDATE_D3D(D3D12CreateDevice(m_pDxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pDevice.GetAddressOf())));
	m_pDevice->SetName(L"Le Device");

	if (_bDebugging)
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
		if (SUCCEEDED(m_pDevice.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		}
	}
	
	const unsigned int kMaxSrvCbvs = 1000;
	if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pDescHeapSrvCbv, kMaxSrvCbvs, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"MainSrvCbvHeap"))
		return false;

	return true;
}

bool DeviceD3D12::InitialiseImGUI(HWND _hWindow, DescriptorHeap* _pSRVHeap)
{
	ImGui_ImplWin32_Init(_hWindow);
	ImGui_ImplDX12_Init(m_pDevice.Get(), 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		_pSRVHeap->m_pDescriptorHeap.Get(), _pSRVHeap->GetCPUStartHandle(), 
		_pSRVHeap->GetGPUStartHandle()
	);
	return true;
}

bool DeviceD3D12::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE _type, CommandQueue** _ppCommandQueue, const wchar_t* _pDebugName)
{
	if (!(*_ppCommandQueue)->Initialise(m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, _pDebugName))
		return false;

	return true;
}

bool DeviceD3D12::CreateCommandList(D3D12_COMMAND_LIST_TYPE _type, CommandList** _ppCommandList, const wchar_t* _pDebugName)
{
	*_ppCommandList = new CommandList();
	if (!(*_ppCommandList)->Initialise(m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, _pDebugName))
		return false;

	return true;
}

bool DeviceD3D12::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap** _ppDescriptorHeap, UINT _numBuffers, D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName)
{
	*_ppDescriptorHeap = new DescriptorHeap();
	if (!(*_ppDescriptorHeap)->Initialise(m_pDevice, _type, _numBuffers, _flags, _pDebugName))
		return false;

	return true;
}

bool DeviceD3D12::CreateSwapChain(SwapChain** _ppSwapChain, CoreWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName)
{
	DescriptorHeap* descHeapRTV;
	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, &descHeapRTV, _numBackBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
		return false;

	DescriptorHeap* descHeapDSV;
	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, &descHeapDSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
		return false;

	*_ppSwapChain = new SwapChain();
	if (!(*_ppSwapChain)->Initialise(m_pDevice, m_pDxgiFactory, CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT), _numBackBuffers, descHeapRTV, descHeapDSV, _pWindow))
		return false;

	return true;
}

bool DeviceD3D12::CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, const wchar_t* _pDebugName)
{
	(*_pTexture) = new Texture2DResource(_pWstrFilename, true, m_pDescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName);

	return true;
}

bool DeviceD3D12::CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, const wchar_t* _pDebugName)
{
	(*_pTexture) = new Texture2DResource(_pWstrFilename, false, m_pDescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName);

	return true;
}

bool DeviceD3D12::CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, VertexBufferResource** _ppResource, const wchar_t* _pDebugName)
{
	(*_ppResource) = new VertexBufferResource(m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);

	return true;
}

bool DeviceD3D12::CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IndexBufferResource** _ppResource, const wchar_t* _pDebugName)
{
	(*_ppResource) = new IndexBufferResource(m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);

	return true;
}

ConstantBufferResource* DeviceD3D12::CreateConstantBufferResource(const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName)
{
	return new ConstantBufferResource(m_pDevice.Get(), m_pDescHeapSrvCbv, _params, _pDebugName);
}

bool DeviceD3D12::CreateRootSignature(D3D12_ROOT_PARAMETER* _pRootParameters, UINT _numParameters, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName)
{
	D3D12_ROOT_SIGNATURE_FLAGS rootSigFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	ZeroMemory(&featureData, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE));
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	VALIDATE_D3D(m_pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(D3D12_FEATURE_DATA_ROOT_SIGNATURE)));

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init_1_0(_numParameters, _pRootParameters, 0, nullptr, rootSigFlags);

	ComPtr<ID3DBlob> pRootSigBlob;
	ComPtr<ID3DBlob> pErrorBlob;
	if (FAILED(D3DX12SerializeVersionedRootSignature(&rootSigDesc, featureData.HighestVersion, &pRootSigBlob, &pErrorBlob)))
	{
		const char* pErrorString = (const char*)pErrorBlob->GetBufferPointer();
		return false;
	}

	VALIDATE_D3D(m_pDevice->CreateRootSignature(0, pRootSigBlob->GetBufferPointer(), pRootSigBlob->GetBufferSize(), IID_PPV_ARGS(_ppRootSignature)));
	(*_ppRootSignature)->SetName(_pDebugName);

	return true;
}

bool DeviceD3D12::CreatePipelineState(PipelineStateDesc _psDesc, ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName)
{
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	ZeroMemory(&rtvFormats, sizeof(D3D12_RT_FORMAT_ARRAY));
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} pipelineStateStream;
	
	pipelineStateStream.VS = _psDesc.VertexShader;
	pipelineStateStream.PS = _psDesc.PixelShader;
	pipelineStateStream.InputLayout = _psDesc.InputLayout;
	pipelineStateStream.pRootSignature = _psDesc.RootSignature;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};

	VALIDATE_D3D(m_pDevice->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(_ppPipelineState)));
	(*_ppPipelineState)->SetName(_pDebugName);

	return true;
}

bool DeviceD3D12::CreateSamplerState(D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName)
{
	m_pDevice->CreateSampler(_pSamplerDesc, _cpuHandle);
	return true;
}

bool DeviceD3D12::FlushState()
{
	return true;
}