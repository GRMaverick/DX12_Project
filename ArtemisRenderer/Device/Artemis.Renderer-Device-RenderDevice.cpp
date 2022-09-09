module;

//#include "CoreWindow.h"

#include "RenderDevice.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "SwapChain.h"

#include "../Helpers/Defines.h"

#include "../Resources/DescriptorHeap.h"
#include "../Resources/ConstantBuffer.h"
#include "../Resources/Texture2D.h"
#include "../Resources/VertexBuffer.h"
#include "../Resources/IndexBuffer.h"
#include "../Resources/GpuResourceTable.h"

#include "../States/SamplerState.h"

#include "../Shaders/ShaderCache.h"

#include <DirectXMath.h>

//#include <ImGUI\imgui_impl_win32.h>
//#include <ImGUI\imgui_impl_dx12.h>
//
//#include "TextureLoader.h"
//#include <WICTextureLoader.h>
//
//#include "SysUtilities\_Profiling\ProfileMarker.h"
//
//#include "SysMemory/include/ScopedMemoryRecord.h"

module Artemis.Renderer:Device;

using namespace DirectX;

import Artemis.Core;

using namespace ArtemisRenderer::States;
using namespace ArtemisRenderer::Shaders;
using namespace ArtemisRenderer::Resources;

namespace ArtemisRenderer::Device
{
	void GenerateInputLayout(IShaderStage* _pShader, std::vector<D3D12_INPUT_ELEMENT_DESC>* _pLayout)
	{
		if (_pShader->GetType() != IShaderStage::ShaderType::VertexShader)
		{
			LogError("Shader generating Input Layout IS NOT a Vertex Shader");
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
		if (m_pDevice) m_pDevice->Release();
		if (m_pDxgiFactory) m_pDxgiFactory->Release();
		if (m_pDxgiAdapter) m_pDxgiAdapter->Release();
	}

	bool DeviceD3D12::Initialise(bool _bDebugging)
	{
		// Query Adapter
		HRESULT hr = S_OK;
		{
			hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_pDxgiFactory));
			if (FAILED(hr))
			{
				assert(false && "DXGI Factory Creation Failed");
				return false;
			}

			SIZE_T maxDedicatedVMEM = 0;
			IDXGIAdapter1* pAdapter1 = nullptr;
			for (UINT adapter = 0; m_pDxgiFactory->EnumAdapters1(adapter, &pAdapter1) != DXGI_ERROR_NOT_FOUND; ++adapter)
			{
				DXGI_ADAPTER_DESC1 adapterDesc1 = {};
				ZeroMemory(&adapterDesc1, sizeof(DXGI_ADAPTER_DESC1));
				pAdapter1->GetDesc1(&adapterDesc1);

				bool bIsSoftwareFlag = (adapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
				bool bCanCreateDevice = SUCCEEDED(D3D12CreateDevice(pAdapter1, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr));
				bool bHasMoreVMEM = adapterDesc1.DedicatedVideoMemory > maxDedicatedVMEM;
				if (bIsSoftwareFlag && bCanCreateDevice && bHasMoreVMEM)
				{
					maxDedicatedVMEM = adapterDesc1.DedicatedVideoMemory;
					//hr = pAdapter1.As(&m_pDxgiAdapter);
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
			ID3D12Debug* spDebugController0;
			ID3D12Debug1* spDebugController1;
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

			ID3D12DeviceRemovedExtendedDataSettings* pDredSettings = nullptr;
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
		VALIDATE_D3D(D3D12CreateDevice(m_pDxgiAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice)));
		m_pDevice->SetName(L"Le Device");

		if (_bDebugging)
		{
			//ID3D12InfoQueue* pInfoQueue = nullptr;
			//if (SUCCEEDED(m_pDevice.As(&pInfoQueue)))
			//{
			//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE);
			//	pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			//}
		}

		const unsigned int kMaxSrvCbvs = 1000;
		if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_DescHeapSrvCbv, kMaxSrvCbvs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"MainSrvCbvHeap"))
			return false;

		const unsigned int kMaxSamplerDescs = 1;
		if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_DescHeapSampler, kMaxSamplerDescs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE))
			return false;

		// Default Sampler
		m_pDefaultSampler = CreateSamplerState(SamplerStateFilter::Linear, SamplerStateWrapMode::Wrap, SamplerStateComparisonFunction::Always);

		return false;
		//return true;
	}

	bool DeviceD3D12::InitialiseImGUI(HWND _hWindow, Resources::DescriptorHeap* _pSRVHeap)
	{
		//ImGui_ImplWin32_Init(_hWindow);
		//ImGui_ImplDX12_Init(m_pDevice.Get(), 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		//	_pSRVHeap->m_pDescriptorHeap.Get(), _pSRVHeap->GetCPUStartHandle(),
		//	_pSRVHeap->GetGPUStartHandle()
		//);
		//return true;
		return false;
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

	bool DeviceD3D12::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE _type, Resources::DescriptorHeap** _pDescriptorHeap, UINT _numBuffers, D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName)
	{
		if (!(*_pDescriptorHeap)) {
			*_pDescriptorHeap = new Resources::DescriptorHeap();
		}

		if (!(*_pDescriptorHeap)->Initialise(m_pDevice, _type, _numBuffers, _flags, _pDebugName))
			return false;

		return true;
	}

	bool DeviceD3D12::CreateSwapChain(SwapChain** _ppSwapChain, SysCore::GameWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName)
	{
		if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, &m_DescHeapRTV, _numBackBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
			return false;

		if (!CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, &m_DescHeapDSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName))
			return false;

		*_ppSwapChain = new SwapChain();
		if (!(*_ppSwapChain)->Initialise(m_pDevice, m_pDxgiFactory, CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT), _numBackBuffers, m_DescHeapRTV, m_DescHeapDSV, _pWindow))
			return false;

		return true;
	}

	States::SamplerState* DeviceD3D12::CreateSamplerState(States::SamplerStateFilter _eFilter, States::SamplerStateWrapMode _eWrap, States::SamplerStateComparisonFunction _eCompFunc)
	{
		States::SamplerState* pSamplerState = new States::SamplerState();
		if (!pSamplerState->Initialise(m_pDevice, m_DescHeapSampler, _eFilter, _eWrap, _eCompFunc))
		{
			delete pSamplerState;
			return nullptr;
		}
		return pSamplerState;
	}

	Resources::IBufferResource* DeviceD3D12::CreateTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName)
	{
		return new Texture2D(_pWstrFilename, true, m_DescHeapSrvCbv, m_pDevice, _pCommandList, _pDebugName);
	}

	Resources::IBufferResource* DeviceD3D12::CreateWICTexture2D(const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName)
	{
		return new Texture2D(_pWstrFilename, false, m_DescHeapSrvCbv, m_pDevice, _pCommandList, _pDebugName);
	}

	Resources::IBufferResource* DeviceD3D12::CreateVertexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, const wchar_t* _pDebugName)
	{
		return new VertexBuffer(m_pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);
	}

	Resources::IBufferResource* DeviceD3D12::CreateIndexBufferResource(CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, void* _pData, const wchar_t* _pDebugName)
	{
		return new IndexBuffer(m_pDevice, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName);
	}

	Resources::IBufferResource* DeviceD3D12::CreateConstantBufferResource(const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName)
	{
		return new ConstantBuffer(m_pDevice, m_DescHeapSrvCbv, _params, _pDebugName);
	}

	bool DeviceD3D12::GetRootSignature(Shaders::IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName)
	{
		GpuResourceTable& resources = *m_DeviceState.Resources;
		ArtemisCore::Hashing::ArtemisHash ulHash = ArtemisCore::Hashing::SimpleHash((const char*)resources.GetVShader()->GetShaderName(), strlen(resources.GetVShader()->GetShaderName()));
		if (m_mapRootSignatures.find(ulHash.Hash) != m_mapRootSignatures.end())
		{
			(*_ppRootSignature) = m_mapRootSignatures[ulHash.Hash];
		}
		else
		{
		//	RenderMarker profile(GetImmediateContext(), "ID3D12Device::CreateRootSignature");

			SamplerState** pSamplers = nullptr;
			IGpuBufferResource** ppBuffers = nullptr, ** ppTextures = nullptr;
			unsigned long ulSamplers = resources.GetSamplers(&pSamplers);
			unsigned long ulTextures = resources.GetTextures(&ppTextures);
			unsigned long ulCBuffers = resources.GetConstantBuffers(&ppBuffers);

			CD3DX12_DESCRIPTOR_RANGE1 Table1[2];
			Table1[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, ulCBuffers, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
			Table1[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ulTextures, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

			CD3DX12_DESCRIPTOR_RANGE1 Table2;
			Table2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, ulSamplers, 0);

			CD3DX12_ROOT_PARAMETER1 RootParameters[2];
			RootParameters[0].InitAsDescriptorTable(ARRAYSIZE(Table1), Table1, D3D12_SHADER_VISIBILITY_ALL);
			RootParameters[1].InitAsDescriptorTable(1, &Table2, D3D12_SHADER_VISIBILITY_ALL);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootSig;
			RootSig.Init_1_1(ARRAYSIZE(RootParameters), RootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
			);

			ID3DBlob* pSerializedRootSig;
			VALIDATE_D3D(D3D12SerializeVersionedRootSignature(&RootSig, &pSerializedRootSig, nullptr));

			ID3D12RootSignature* pRootSig = nullptr;
			VALIDATE_D3D(m_pDevice->CreateRootSignature(0, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(&pRootSig)));
			pRootSig->SetName(_pDebugName);

			m_mapRootSignatures[ulHash.Hash] = pRootSig;
			(*_ppRootSignature) = pRootSig;
		}

		return true;
	}

	bool DeviceD3D12::GetPipelineState(ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName)
	{
		if (!m_DeviceState.Resources)
		{
			LogError("No Valid Resource Table Flushed to Device State");
			return false;
		}

		ID3D12RootSignature* pRootSignature = nullptr;
		if (!GetRootSignature(m_DeviceState.Resources->GetVShader(), &pRootSignature))
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
		GenerateInputLayout(m_DeviceState.Resources->GetVShader(), &inputLayout);

		unsigned long long ulHash = 0;
		// Parameterised
		{
			LOW_LEVEL_PROFILE_MARKER(GetImmediateContext(), "GetPipelineState::Hash Ident");

			stateDesc.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
			stateDesc.pRootSignature = pRootSignature;
			stateDesc.VS = { m_DeviceState.Resources->GetVShader()->GetBytecode(), m_DeviceState.Resources->GetVShader()->GetBytecodeSize() };
			stateDesc.PS = { m_DeviceState.Resources->GetPShader()->GetBytecode(), m_DeviceState.Resources->GetPShader()->GetBytecodeSize() };

			ulHash += ArtemisCore::Hashing::SimpleHash((const char*)&inputLayout[0], sizeof(D3D12_INPUT_ELEMENT_DESC) * inputLayout.size()).Hash;
			ulHash += ArtemisCore::Hashing::SimpleHash((const char*)&stateDesc.pRootSignature, sizeof(ID3D12RootSignature)).Hash;
			ulHash += ArtemisCore::Hashing::SimpleHash((const char*)m_DeviceState.Resources->GetVShader()->GetShaderName(), strlen(m_DeviceState.Resources->GetVShader()->GetShaderName())).Hash;
			ulHash += ArtemisCore::Hashing::SimpleHash((const char*)m_DeviceState.Resources->GetPShader()->GetShaderName(), strlen(m_DeviceState.Resources->GetPShader()->GetShaderName())).Hash;
		}

		if (m_mapPSO.find(ulHash) != m_mapPSO.end())
		{
			(*_ppPipelineState) = m_mapPSO[ulHash];
		}
		else
		{
			LOW_LEVEL_PROFILE_MARKER(GetImmediateContext(), "ID3D12Device::CreatePipelineState");

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
		m_pImmediateContext = CommandList::Build(D3D12_COMMAND_LIST_TYPE_DIRECT, L"ImmediateContext");

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
		m_ActiveResourceHeap->~DescriptorHeap();
		m_ActiveSamplerHeap->~DescriptorHeap();
	}

	bool DeviceD3D12::FlushState()
	{
		CommandList* pGfxCmdList = GetImmediateContext();
		GpuResourceTable& Resources = *m_DeviceState.Resources;

		LOW_LEVEL_PROFILE_MARKER(pGfxCmdList, "FlushState");

		ID3D12PipelineState* pPSO = nullptr;
		if (!GetPipelineState(&pPSO))
			return false;

		pGfxCmdList->SetPipelineState(pPSO);

		ID3D12DescriptorHeap* pHeaps[] = { m_ActiveResourceHeap->GetHeap(), m_ActiveSamplerHeap->GetHeap() };
		pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

		unsigned int uiResourceStartIndex = m_ActiveResourceHeap->GetFreeIndex();

		//
		// Copy CBVs
		//
		Resources::IGpuBufferResource** ppCBs = nullptr;
		unsigned long ulResources = Resources.GetConstantBuffers(&ppCBs);
		for (unsigned int i = 0; i < ulResources; ++i)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource(m_ActiveResourceHeap->GetCPUStartHandle(), m_ActiveResourceHeap->GetFreeIndexAndIncrement(), m_ActiveResourceHeap->GetIncrementSize());

			if (!ppCBs[i])
			{
				LogWarning("Null Constant Buffer");
				continue;
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppCBs[i]->GetCpuHandle();

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER(pGfxCmdList, "CBV Desc Copies");
			m_pDevice->CopyDescriptors(
				1, &hCpuNewResource, &size1,
				1, &hCpuActual, &size2,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		// 
		// Copy SRVs
		// 
		Resources::IGpuBufferResource** ppTextures = nullptr;
		ulResources = Resources.GetTextures(&ppTextures);
		for (unsigned int i = 0; i < ulResources; ++i)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource(m_ActiveResourceHeap->GetCPUStartHandle(), m_ActiveResourceHeap->GetFreeIndexAndIncrement(), m_ActiveResourceHeap->GetIncrementSize());

			if (!ppTextures[i])
			{
				LogWarning("Invalid Texture");

				static D3D12_SHADER_RESOURCE_VIEW_DESC staticNullDesc;
				staticNullDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				staticNullDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				staticNullDesc.Texture2D.MipLevels = 0;
				staticNullDesc.Texture2D.MostDetailedMip = 0;
				staticNullDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				staticNullDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				m_pDevice->CreateShaderResourceView(nullptr, &staticNullDesc, hCpuNewResource);

				continue;
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppTextures[i]->GetCpuHandle();

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER(pGfxCmdList, "SRV Desc Copies");
			m_pDevice->CopyDescriptors(
				1, &hCpuNewResource, &size1,
				1, &hCpuActual, &size2,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		//
		// Copy Sampler
		//
		States::SamplerState** pSamplers = nullptr;
		ulResources = Resources.GetSamplers(&pSamplers);

		unsigned int uiSamplerStartIndex = m_ActiveSamplerHeap->GetFreeIndex();
		for (unsigned int i = 0; i < ulResources; ++i)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE sampDescHandle(m_DescHeapSampler->GetCPUStartHandle());
			sampDescHandle.Offset(((States::SamplerState*)pSamplers[i])->GetHeapIndex(), m_DescHeapSampler->GetIncrementSize());

			CD3DX12_CPU_DESCRIPTOR_HANDLE tempHandleLoc(m_ActiveSamplerHeap->GetCPUStartHandle(), m_ActiveSamplerHeap->GetFreeIndexAndIncrement(), m_ActiveSamplerHeap->GetIncrementSize());

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER(pGfxCmdList, "Sampler Desc Copies");
			m_pDevice->CopyDescriptors(
				1, &tempHandleLoc, &size1,
				1, &sampDescHandle, &size2,
				D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		}

		pGfxCmdList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_ActiveResourceHeap->GetGPUStartHandle(), uiResourceStartIndex, m_ActiveResourceHeap->GetIncrementSize()));
		pGfxCmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_ActiveSamplerHeap->GetGPUStartHandle(), uiSamplerStartIndex, m_ActiveSamplerHeap->GetIncrementSize()));

		m_DeviceState.DirtyFlags = 0;

		return true;
	}

	bool DeviceD3D12::SetMaterial(const char* _pName)
	{
		unsigned long long ulHash = ArtemisCore::Hashing::SimpleHash(_pName, strlen(_pName)).Hash;
		if (m_mapGpuResourceTables.find(ulHash) != m_mapGpuResourceTables.end())
		{
			m_DeviceState.Resources = m_mapGpuResourceTables[ulHash];
			return true;
		}

		Effect* set = ShaderCache::Instance()->GetEffect(_pName);
		if (!set)
		{
			return false;
		}

		GpuResourceTable* pGpuResourceTable = new GpuResourceTable(set->GetVertexShader(), set->GetPixelShader());
		m_DeviceState.Resources = pGpuResourceTable;
		m_mapGpuResourceTables[ulHash] = pGpuResourceTable;

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

	bool DeviceD3D12::SetTexture(const char* _pName, Resources::IGpuBufferResource* _pTexture)
	{
		if (!m_DeviceState.Resources)
		{
			LogError("SetTexture - No Valid Resource Table Flushed to Device State");
			return false;
		}

		return m_DeviceState.Resources->SetTexture(_pName, _pTexture);
	}

	bool DeviceD3D12::SetConstantBuffer(const char* _pName, Resources::IGpuBufferResource* _pCBuffer)
	{
		if (!m_DeviceState.Resources)
		{
			LogError("SetConstantBuffer - No Valid Resource Table Flushed to Device State");
			return false;
		}

		return m_DeviceState.Resources->SetConstantBuffer(_pName, _pCBuffer);
	}

	bool DeviceD3D12::SetSamplerState(const char* _pName, States::SamplerState* _pSamplerState)
	{
		if (!m_DeviceState.Resources)
		{
			LogError("SetSamplerState - No Valid Resource Table Flushed to Device State");
			return false;
		}

		return m_DeviceState.Resources->SetSamplerState(_pName, _pSamplerState);
	}
}