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
#include "D3D12\Resources\UploadBuffer.h"

#include <assert.h>
#include <DirectXMath.h>

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#include "TextureLoader.h"
#include <WICTextureLoader.h>

using namespace DirectX;
using namespace Microsoft::WRL;

PRAGMA_TODO("Debug Flags for Device Creation")
PRAGMA_TODO("\tDRED Features")
PRAGMA_TODO("\tInfo Queue Features");

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
#if 0
		// TODO: DRED Features
		ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings));
		if (FAILED(hr))
		{
			assert(false && "DRED settings failed");
		}
		else
		{
			pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}

		spDebugController1->SetEnableGPUBasedValidation(true);

		// TODO: Info Queue Features
		//ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
		//if (SUCCEEDED(m_pDevice.As(&pInfoQueue)))
		//{
		//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		//}
		//else
		//{
		//	assert(false && "Debugging Device Creation Failed");
		//}
#endif
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

bool DeviceD3D12::UploadResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData,IBufferResource** _ppResource, const wchar_t* _pDebugName)
{
	HRESULT hr = S_OK;

	UINT bufferSize = _sizeInBytes * _strideInBytes;
	ComPtr<ID3D12Resource> pDestination = nullptr;
	hr = m_pDevice->CreateCommittedResource( 
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
		D3D12_HEAP_FLAG_NONE, 
		&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, _flags),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr, 
		IID_PPV_ARGS(pDestination.GetAddressOf())
	);
	if (FAILED(hr))
	{
		assert(false && "Destination Buffer Setup Failed");
		return false;
	}
	if (pDestination.Get())
		(*_ppResource)->SetGPUBuffer(pDestination);

	if (_pData)
	{
		ComPtr<ID3D12Resource> pIntermediate = nullptr;
		hr = m_pDevice->CreateCommittedResource( &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(pIntermediate.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "Upload Buffer Setup Failed");
			return false;
		}
		if (pIntermediate.Get())
			(*_ppResource)->SetCPUBuffer(pIntermediate);

		D3D12_SUBRESOURCE_DATA srData = {};
		ZeroMemory(&srData, sizeof(D3D12_SUBRESOURCE_DATA));
		srData.pData = _pData;
		srData.RowPitch = bufferSize;
		srData.SlicePitch = srData.RowPitch;

		_pCommandList->UpdateSubresource((*_ppResource)->GetGPUBuffer().Get(), (*_ppResource)->GetCPUBuffer().Get(), 0, 0, 1, &srData);
	}

	return true;
}

bool DeviceD3D12::CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, DescriptorHeap* _pDescHeapSRV, const wchar_t* _pDebugName)
{
	(*_pTexture) = new Texture2DResource();
	(*_pTexture)->Initialise(_pDescHeapSRV->GetFreeIndex());
	_pDescHeapSRV->Increment();

	ComPtr<ID3D12Resource> pCPUTexture = nullptr;
	ComPtr<ID3D12Resource> pGPUTexture = nullptr;
	VALIDATE_D3D(CreateDDSTextureFromFile12(m_pDevice.Get(), _pCommandList, _pWstrFilename, pGPUTexture, pCPUTexture));
		
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = pCPUTexture->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = pCPUTexture->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(_pDescHeapSRV->GetCPUStartHandle());
	m_pDevice->CreateShaderResourceView(pGPUTexture.Get(), &srvDesc, srvHandle);

	wchar_t pCPUDebugName[256];
	wchar_t pGPUDebugName[256];

	wsprintf(pCPUDebugName, L"%s_%s", _pDebugName, L"CPU");
	wsprintf(pGPUDebugName, L"%s_%s", _pDebugName, L"GPU");

	pCPUTexture->SetName(pCPUDebugName);
	pGPUTexture->SetName(pGPUDebugName);

	(*_pTexture)->SetCPUBuffer(pCPUTexture);
	(*_pTexture)->SetGPUBuffer(pGPUTexture);
	
	return true;
}

bool DeviceD3D12::CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, Texture2DResource** _pTexture, DescriptorHeap* _pDescHeapSRV, const wchar_t* _pDebugName)
{
	ComPtr<ID3D12Resource> pCPUTexture = nullptr;
	ComPtr<ID3D12Resource> pGPUTexture = nullptr;

	D3D12_SUBRESOURCE_DATA srData; 
	std::unique_ptr<uint8_t[]> decodedData;
	if (FAILED(LoadWICTextureFromFile(m_pDevice.Get(), _pWstrFilename, pGPUTexture.GetAddressOf(), decodedData, srData)))
		return false;

	VALIDATE_D3D(m_pDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(srData.SlicePitch),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pCPUTexture)
	));

	_pCommandList->UpdateSubresource(pGPUTexture.Get(), pCPUTexture.Get(), 0, 0, 1, &srData);

	(*_pTexture) = new Texture2DResource();
	(*_pTexture)->Initialise(_pDescHeapSRV->GetFreeIndex());
	_pDescHeapSRV->Increment();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = pCPUTexture->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = pCPUTexture->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(_pDescHeapSRV->GetCPUStartHandle());
	srvHandle.Offset((*_pTexture)->GetHeapIndex(), _pDescHeapSRV->GetIncrementSize());

	m_pDevice->CreateShaderResourceView(pGPUTexture.Get(), &srvDesc, srvHandle);


	if (_pDebugName)
	{
		wchar_t pCPUDebugName[256];
		wchar_t pGPUDebugName[256];
		wsprintf(pCPUDebugName, L"%s_%s", _pDebugName, L"CPU");
		wsprintf(pGPUDebugName, L"%s_%s", _pDebugName, L"GPU");
		pCPUTexture->SetName(pCPUDebugName);
		pGPUTexture->SetName(pGPUDebugName);
	}

	(*_pTexture)->SetCPUBuffer(pCPUTexture);
	(*_pTexture)->SetGPUBuffer(pGPUTexture);

	return true;
}

bool DeviceD3D12::CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, VertexBufferResource** _ppResource, const wchar_t* _pDebugName)
{
	*_ppResource = new VertexBufferResource();
	if (!UploadResource(_pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, (IBufferResource**)_ppResource))
		return false;

	D3D12_VERTEX_BUFFER_VIEW vbv = { };
	ZeroMemory(&vbv, sizeof(D3D12_VERTEX_BUFFER_VIEW));
	vbv.BufferLocation = (*_ppResource)->GetGPUBuffer()->GetGPUVirtualAddress();
	vbv.SizeInBytes = _sizeInBytes * _strideInBytes;
	vbv.StrideInBytes = _strideInBytes;

	(*_ppResource)->SetView(vbv);
	return true;
}
bool DeviceD3D12::CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, IndexBufferResource** _ppResource, const wchar_t* _pDebugName)
{
	*_ppResource = new IndexBufferResource();
	if (!UploadResource(_pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, (IBufferResource**)_ppResource))
		return false;

	D3D12_INDEX_BUFFER_VIEW ibv = { };
	ZeroMemory(&ibv, sizeof(D3D12_INDEX_BUFFER_VIEW));
	ibv.BufferLocation = (*_ppResource)->GetGPUBuffer()->GetGPUVirtualAddress();
	ibv.SizeInBytes = _sizeInBytes * _strideInBytes;
	ibv.Format = DXGI_FORMAT_R32_UINT;

	(*_ppResource)->SetView(ibv);
	return true;
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