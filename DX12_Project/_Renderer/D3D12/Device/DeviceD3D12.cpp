#include "Defines.h"

#include "CoreWindow.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"
#include "D3D12\Device\SwapChain.h"

#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\Texture2DResource.h"
#include "D3D12\Resources\VertexBufferResource.h"
#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\ConstantBufferResource.h"
#include "D3D12\Resources\UploadBuffer.h"

#include "D3D12\Shaders\ShaderCache.h"

#include <assert.h>
#include <DirectXMath.h>

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "ProfileMarker.h"

#include "SysMemory/include/ScopedMemoryRecord.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysMemory;

unsigned long HashString(char* _pObject, size_t _szlength)
{
	unsigned long hash = 5381;

	int c = 0;
	for (unsigned int i = 0; i < _szlength; ++i)
	{
		int c = _pObject[i];
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}


void GenerateInputLayout(IShader* _pShader, std::vector<D3D12_INPUT_ELEMENT_DESC>* _pLayout)
{
	if (_pShader->GetType() != IShader::ShaderType::VertexShader)
	{
		LogError_Renderer("Shader generating Input Layout IS NOT a Vertex Shader");
		return;
	}

	ShaderIOParameters parameters = _pShader->GetShaderParameters();

	_pLayout->reserve(parameters.NumberInputs);

	for (unsigned int input = 0; input < parameters.NumberInputs; ++input)
	{
		const ShaderIOParameters::Parameter& p = parameters.Inputs[input];
		D3D12_INPUT_ELEMENT_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_INPUT_ELEMENT_DESC));
		desc.SemanticIndex = p.SemanticIndex;
		desc.SemanticName = p.SemanticName;
		desc.InputSlot = 0;
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		if (p.Mask == 1)
		{
			if (p.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				desc.Format = DXGI_FORMAT_R32_UINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				desc.Format = DXGI_FORMAT_R32_SINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				desc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (p.Mask <= 3)
		{
			if (p.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				desc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				desc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				desc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (p.Mask <= 7)
		{
			if (p.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				desc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				desc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (p.Mask <= 15)
		{
			if (p.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		_pLayout->push_back(desc);
	}
}

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
	if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_DescHeapSrvCbv, kMaxSrvCbvs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"MainSrvCbvHeap"))
		return false;

	const unsigned int kMaxSamplerDescs = 1;
	if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_DescHeapSampler, kMaxSamplerDescs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
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

bool DeviceD3D12::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap* _ppDescriptorHeap, UINT _numBuffers, D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName)
{
	*_ppDescriptorHeap = DescriptorHeap();
	if (!_ppDescriptorHeap->Initialise(m_pDevice, _type, _numBuffers, _flags, _pDebugName))
		return false;

	return true;
}

bool DeviceD3D12::CreateSwapChain(SwapChain** _ppSwapChain, CoreWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName)
{
	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, &m_DescHeapRTV, _numBackBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
		return false;

	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, &m_DescHeapDSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
		return false;

	*_ppSwapChain = new SwapChain();
	if (!(*_ppSwapChain)->Initialise(m_pDevice, m_pDxgiFactory, CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT), _numBackBuffers, &m_DescHeapRTV, &m_DescHeapDSV, _pWindow))
		return false;

	return true;
}

IBufferResource* DeviceD3D12::CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName)
{
	return new Texture2DResource(_pWstrFilename, true, &m_DescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName);
}

IBufferResource* DeviceD3D12::CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName)
{
	return new Texture2DResource(_pWstrFilename, false, &m_DescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName);
}

IBufferResource* DeviceD3D12::CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, const wchar_t* _pDebugName)
{
	return new VertexBufferResource(m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);
}

IBufferResource* DeviceD3D12::CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, const wchar_t* _pDebugName)
{
	return new IndexBufferResource(m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);
}

ConstantBufferResource* DeviceD3D12::CreateConstantBufferResource(const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName)
{
	return new ConstantBufferResource(m_pDevice.Get(), &m_DescHeapSrvCbv, _params, _pDebugName);
}

bool DeviceD3D12::GetRootSignature(IShader* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName)
{
	if (_pShader->GetType() != IShader::ShaderType::VertexShader)
	{
		assert(false && "Shader is not a VertexShader");
		return false;
	}

	unsigned long ulHash = HashString((char*)_pShader->GetShaderName(), strlen(_pShader->GetShaderName()));
	if (m_mapRootSignatures.find(ulHash) != m_mapRootSignatures.end())
	{
		(*_ppRootSignature) = m_mapRootSignatures[ulHash];
	}
	else
	{
		RenderMarker profile(GetImmediateContext(), "ID3D12Device::CreateRootSignature");
		ID3D12RootSignature* pRootSig = nullptr;
		VALIDATE_D3D(m_pDevice->CreateRootSignature(0, _pShader->GetBytecode(), _pShader->GetBytecodeSize(), IID_PPV_ARGS(&pRootSig)));
		pRootSig->SetName(_pDebugName);

		m_mapRootSignatures[ulHash] = pRootSig;
		(*_ppRootSignature) = pRootSig;
	}

	return true;
}

bool DeviceD3D12::GetPipelineState(ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName)
{
	ID3D12RootSignature* pRootSignature = nullptr;
	if (!GetRootSignature(m_DeviceState.Resources.GetVShader(), &pRootSignature))
	{
		assert(false && "Root Sig Creation Failure");
	}

	GetImmediateContext()->SetGraphicsRootSignature(pRootSignature);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc;
	ZeroMemory(&stateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	// Default
	{
		stateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		stateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		stateDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		stateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		stateDesc.SampleMask = UINT_MAX;
		stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		PRAGMA_TODO("Change Formats on Changed RTV/DSV");
		stateDesc.NumRenderTargets = 1;
		stateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		stateDesc.SampleDesc.Count = 1;
	}

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	GenerateInputLayout(m_DeviceState.Resources.GetVShader(), &inputLayout);

	unsigned long ulHash = 0;
	// Parameterised
	{
		RenderMarker profile(GetImmediateContext(), "GetPipelineState::Hash Ident");

		stateDesc.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		stateDesc.pRootSignature = pRootSignature;
		stateDesc.VS = { m_DeviceState.Resources.GetVShader()->GetBytecode(), m_DeviceState.Resources.GetVShader()->GetBytecodeSize() };
		stateDesc.PS = { m_DeviceState.Resources.GetPShader()->GetBytecode(), m_DeviceState.Resources.GetPShader()->GetBytecodeSize() };
		
		ulHash += HashString((char*)&inputLayout[0], sizeof(D3D12_INPUT_ELEMENT_DESC) * inputLayout.size());
		ulHash += HashString((char*)&stateDesc.pRootSignature, sizeof(ID3D12RootSignature));
		ulHash += HashString((char*)m_DeviceState.Resources.GetVShader()->GetShaderName(), strlen(m_DeviceState.Resources.GetVShader()->GetShaderName()));
		ulHash += HashString((char*)m_DeviceState.Resources.GetPShader()->GetShaderName(), strlen(m_DeviceState.Resources.GetPShader()->GetShaderName()));
	}
	
	if (m_mapPSO.find(ulHash) != m_mapPSO.end())
	{
		(*_ppPipelineState) = m_mapPSO[ulHash];
	}
	else
	{
		RenderMarker profile(GetImmediateContext(), "ID3D12Device::CreatePipelineState");

		ID3D12PipelineState* pPSO = nullptr;
		VALIDATE_D3D(m_pDevice->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(&pPSO)));
		pPSO->SetName(_pDebugName);

		m_mapPSO[ulHash] = pPSO;
		(*_ppPipelineState) = pPSO;
	}

	return true;
}

bool DeviceD3D12::CreateSamplerState(D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName)
{
	m_pDevice->CreateSampler(_pSamplerDesc, _cpuHandle);
	return true;
}

void DeviceD3D12::BeginFrame(void)
{
	m_ImmediateContext = CommandList::Build(D3D12_COMMAND_LIST_TYPE_DIRECT, L"ImmediateContext");
	
	const unsigned int kElements = 2000;
	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_ActiveResourceHeap, kElements, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"Temp Texture Heap"))
	{
		assert(false && "Temp Texture Heap Creation Failure");
	}

	if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_ActiveSamplerHeap, kElements, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"Temp Sampler Heap"))
	{
		assert(false && "Temp Texture Heap Creation Failure");
	}
}

void DeviceD3D12::EndFrame(void)
{
	m_ActiveResourceHeap.~DescriptorHeap();
	m_ActiveSamplerHeap.~DescriptorHeap();
}

bool DeviceD3D12::FlushState()
{
	CommandList* pGfxCmdList = GetImmediateContext();
	GpuResourceTable& Resources = m_DeviceState.Resources;

	RenderMarker profile(pGfxCmdList, "FlushState");

	ID3D12PipelineState* pPSO = nullptr;
	if (!GetPipelineState(&pPSO))
		return false;

	pGfxCmdList->SetPipelineState(pPSO);

	ID3D12DescriptorHeap* pHeaps[] = { m_ActiveResourceHeap.GetHeap(), m_ActiveSamplerHeap.GetHeap() };
	pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

	unsigned int uiResourceStartIndex = m_ActiveResourceHeap.GetFreeIndex();

	//
	// Copy CBVs
	//
	IGpuBufferResource** ppCBs = nullptr;
	unsigned long ulResources = Resources.GetConstantBuffers(&ppCBs);
	for (unsigned int i = 0; i < ulResources; ++i)
	{
		if (!ppCBs[i])
		{
			assert(false && "Invalid CB");
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppCBs[i]->GetCpuHandle();
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource(m_ActiveResourceHeap.GetCPUStartHandle(), m_ActiveResourceHeap.GetFreeIndexAndIncrement(), m_ActiveResourceHeap.GetIncrementSize());

		UINT size1 = 1;
		UINT size2 = 1;

		RenderMarker profile(pGfxCmdList, "CBV Desc Copies");
		m_pDevice->CopyDescriptors(
			1, &hCpuNewResource, &size1,
			1, &hCpuActual, &size2,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// 
	// Copy SRVs
	// 
	IGpuBufferResource** ppTextures = nullptr;
	ulResources = Resources.GetTextures(&ppTextures);
	for (unsigned int i = 0; i < ulResources; ++i)
	{
		if (!ppTextures[i])
		{
			LogWarning_Renderer("Invalid Texture");
			return false;
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppTextures[i]->GetCpuHandle();
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource(m_ActiveResourceHeap.GetCPUStartHandle(), m_ActiveResourceHeap.GetFreeIndexAndIncrement(), m_ActiveResourceHeap.GetIncrementSize());

		UINT size1 = 1;
		UINT size2 = 1;

		RenderMarker profile(pGfxCmdList, "SRV Desc Copies");
		m_pDevice->CopyDescriptors(
			1, &hCpuNewResource, &size1,
			1, &hCpuActual, &size2,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//
	// Copy Sampler
	//
	unsigned int uiSamplerStartIndex = m_ActiveSamplerHeap.GetFreeIndex();
	for (unsigned int i = 0; i < ARRAYSIZE(m_DeviceState.Sampler); ++i)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE sampDescHandle(m_DescHeapSampler.GetCPUStartHandle());
		sampDescHandle.Offset(m_DeviceState.Sampler[i].HeapIndex, m_DescHeapSampler.GetIncrementSize());
		CD3DX12_CPU_DESCRIPTOR_HANDLE tempHandleLoc(m_ActiveSamplerHeap.GetCPUStartHandle(), m_ActiveSamplerHeap.GetFreeIndexAndIncrement(), m_ActiveSamplerHeap.GetIncrementSize());

		UINT size1 = 1;
		UINT size2 = 1;

		RenderMarker profile(pGfxCmdList, "Sampler Desc Copies");
		m_pDevice->CopyDescriptors(
			1, &tempHandleLoc, &size1,
			1, &sampDescHandle, &size2,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

	pGfxCmdList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_ActiveResourceHeap.GetGPUStartHandle(), uiResourceStartIndex, m_ActiveResourceHeap.GetIncrementSize()));
	pGfxCmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_ActiveSamplerHeap.GetGPUStartHandle(), uiSamplerStartIndex, m_ActiveSamplerHeap.GetIncrementSize()));

	m_DeviceState.DirtyFlags = 0;

	return true;
}

bool DeviceD3D12::SetMaterial(const char* _pName)
{
	ShaderSet set = ShaderCache::Instance()->GetShader(_pName);
	
	m_DeviceState.Resources.Initialise(set.VertexShader, set.PixelShader);

	return true;
}

bool DeviceD3D12::SetRenderTarget(void)
{
	return true;
}

bool DeviceD3D12::SetDepthBuffer(void)
{
	return true;
}

bool DeviceD3D12::SetTexture(const char* _pName, IGpuBufferResource* _pTexture)
{
	return m_DeviceState.Resources.SetTexture(_pName, _pTexture);
}

bool DeviceD3D12::SetConstantBuffer(const char* _pName, IGpuBufferResource* _pCBuffer)
{
	return m_DeviceState.Resources.SetConstantBuffer(_pName, _pCBuffer);
}

bool DeviceD3D12::SetSamplerState(const char* _pName, D3D12_SAMPLER_DESC _state)
{
	unsigned long ulHash = HashString((char*)&_state, sizeof(D3D12_SAMPLER_DESC));
	if (m_mapSamplers.find(ulHash) != m_mapSamplers.end())
	{
		m_DeviceState.Resources.SetSamplerState(_pName, m_mapSamplers[ulHash]);
	}
	else
	{
		m_DeviceState.DirtyFlags |= kDirtySamplerState;

		SamplerStateEntry newState;
		newState.Hash = ulHash;
		newState.HeapIndex = m_DescHeapSampler.GetFreeIndex();

		m_mapSamplers[ulHash] = newState;

		CD3DX12_CPU_DESCRIPTOR_HANDLE sampHandle(m_DescHeapSampler.GetCPUStartHandle());
		sampHandle.Offset(newState.HeapIndex, m_DescHeapSampler.GetIncrementSize());

		if (!CreateSamplerState(&_state, sampHandle))
			return false;

		m_DeviceState.Resources.SetSamplerState(_pName, m_mapSamplers[ulHash]);
	}

	return true;
}