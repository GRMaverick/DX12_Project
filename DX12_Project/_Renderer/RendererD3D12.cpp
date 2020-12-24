#include "Defines.h"

#include "CoreWindow.h"
#include "RendererD3D12.h"
#include "AssimpLoader.h"
#include "ImGUI\ImGUIEngine.h"

#include "d3dx12.h"

#include <assert.h>
#include <DirectXMath.h>

#include <WICTextureLoader.h>

#include "Scene\RenderEntity.h"

#include "ProfileMarker.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

#define SHADER_CACHE_LOCATION "Shaders\\*"

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

RendererD3D12::RendererD3D12(void)
{
	m_pSwapChain = nullptr;
}

RendererD3D12::~RendererD3D12(void)
{
	if (m_pSwapChain) delete m_pSwapChain; m_pSwapChain = nullptr;
}

bool RendererD3D12::Initialise(CoreWindow* _pWindow)
{
	if (!DeviceD3D12::Instance()->Initialise(true))
		return false;

	m_ShaderCache = ShaderCache(SHADER_CACHE_LOCATION);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY);

	if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pImGuiSRVHeap, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ImGUI SRV"))
		return false;

	ImGUIEngine::Initialise(_pWindow->GetWindowHandle(), m_pImGuiSRVHeap);

	if (!DeviceD3D12::Instance()->CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, L"Swap Chain"))
		return false;
	
	if (!LoadContent())
		return false;

	if (!CreatePipelineState())
		return false;

	return true;
}

const char* g_ModelList[] =
{
	"Content\\AnalogMeter.Needle.Dark\\AnalogMeter.fbx",
	//"Content\\Sponza\\Sponza.fbx",
	//"Content\\S&W_45ACP\\Handgun_fbx_7.4_binary.fbx",
	//"Content\\Room\\OBJ\\Room.obj",
	//"Content\\Cube\\Cube.obj",
};

bool RendererD3D12::LoadContent(void)
{	
	m_bNewModelsLoaded = true;

	LogInfo_Renderer("Loading Models:");
	m_pRenderEntity = new RenderEntity*[_countof(g_ModelList)];
	for (UINT i = 0; i < _countof(g_ModelList); ++i)
	{
		LogInfo_Renderer("\t%s", g_ModelList[i]);

		m_pRenderEntity[i] = new RenderEntity();
		m_pRenderEntity[i]->LoadModelFromFile(g_ModelList[i]);
		m_pRenderEntity[i]->SetScale(1.0f);
		m_pRenderEntity[i]->SetRotation(0.0f, 0.0f, 0.0f);
		m_pRenderEntity[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_ModelCount++;
	}

	m_Camera.SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera.SetUp(0.0f, 1.0f, 0.0f);
	m_Camera.SetTarget(0.0f, 0.0f, 0.0f);
	m_Camera.SetFieldOfView(45.0f);
	m_Camera.SetAspectRatio(1920.0f / 1080.0f);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandLists();
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->Flush();

	return true;
}

void GenerateInputLayout(IShader* _pShader, std::vector<D3D12_INPUT_ELEMENT_DESC>* _pLayout)
{
	if (_pShader->GetType() != IShader::ShaderType::VertexShader)
	{
		LogError_Renderer("Shader generating Input Layout IS NOT a Vertex Shader");
		return;
	}

	ShaderIOParameters parameters = _pShader->GetParameters();

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

bool RendererD3D12::CreatePipelineState(void)
{
	// Basic Colour PSO
	{
		CD3DX12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		if (!DeviceD3D12::Instance()->CreateRootSignature(rootParameters, _countof(rootParameters), m_pBasicRS.GetAddressOf()))
			return false;
						
		ShaderSet set = m_ShaderCache.GetShader("Basic");

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		GenerateInputLayout(set.VertexShader, &inputLayout);

		PipelineStateDesc psDesc;
		ZeroMemory(&psDesc, sizeof(PipelineStateDesc));
		psDesc.VertexShader = { set.VertexShader->GetBytecode(), set.VertexShader->GetBytecodeSize() };
		psDesc.PixelShader = { set.PixelShader->GetBytecode(), set.PixelShader->GetBytecodeSize() };
		psDesc.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		psDesc.RootSignature = m_pBasicRS.Get();

		if (!DeviceD3D12::Instance()->CreatePipelineState(psDesc, m_pBasicPSO.GetAddressOf()))
			return false;
	}

	// Albedo PSO
	{
		if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_pDescHeapSampler, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
			return false;

		D3D12_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		if (!DeviceD3D12::Instance()->CreateSamplerState(&samplerDesc, m_pDescHeapSampler->GetCPUStartHandle()))
			return false;

		CD3DX12_DESCRIPTOR_RANGE srvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		CD3DX12_DESCRIPTOR_RANGE samplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

		CD3DX12_ROOT_PARAMETER rootParameters[4];
		rootParameters[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX); // Pass
		rootParameters[3].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX); // Object

		if (!DeviceD3D12::Instance()->CreateRootSignature(rootParameters, _countof(rootParameters), m_pAlbedoRS.GetAddressOf()))
			return false;

		ShaderSet set = m_ShaderCache.GetShader("Albedo");

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		GenerateInputLayout(set.VertexShader, &inputLayout);

		PipelineStateDesc psDesc;
		ZeroMemory(&psDesc, sizeof(PipelineStateDesc));
		psDesc.VertexShader = { set.VertexShader->GetBytecode(), set.VertexShader->GetBytecodeSize() };
		psDesc.PixelShader = { set.PixelShader->GetBytecode(), set.PixelShader->GetBytecodeSize() };
		psDesc.InputLayout = { &inputLayout[0], (UINT)inputLayout.size() };
		psDesc.RootSignature = m_pAlbedoRS.Get();

		if (!DeviceD3D12::Instance()->CreatePipelineState(psDesc, m_pAlbedoPSO.GetAddressOf()))
			return false;
	}

	return true;
}

void RendererD3D12::Update(double _deltaTime)
{
	UpdatePassConstants();

	if (m_bNewModelsLoaded)
	{
		m_bNewModelsLoaded = false;

		UINT numNewCBs = 0;
		for (UINT i = 0; i < m_ModelCount; ++i)
		{
			numNewCBs += m_pRenderEntity[i]->GetModel()->MeshCount;
		}

		if (m_ObjectCBs)
			delete m_ObjectCBs;

		m_ObjectCBs = new UploadBuffer<ObjectCB>(DeviceD3D12::Instance()->m_pDevice.Get(), numNewCBs, true);
		m_ObjectCBCount = numNewCBs;
	}

	UINT cbIndex = 0;
	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		m_pRenderEntity[i]->Update();
		for (int j = 0; j < m_pRenderEntity[i]->GetModel()->MeshCount; ++j)
		{
			ObjectCB objectCb;
			objectCb.MVP = m_pRenderEntity[i]->GetWorld() * m_Camera.GetView() * m_Camera.GetProjection();
			m_ObjectCBs->CopyData(cbIndex, objectCb);
			cbIndex++;
		}
	}
}

void RendererD3D12::UpdatePassConstants()
{
	m_Camera.Update();
}

bool RendererD3D12::Render(void)
{
	CommandList* pGfxCmdList = CommandList::Build(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Rendering
	{
		m_pSwapChain->PrepareForRendering(pGfxCmdList);
		m_pSwapChain->SetOMRenderTargets(pGfxCmdList);
		pGfxCmdList->SetIAPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		MainRenderPass(pGfxCmdList);

		// ImGui
		{
			RenderMarker profile(pGfxCmdList, "%s", "ImGUI");

			ID3D12DescriptorHeap* pHeaps[] = { m_pImGuiSRVHeap->GetHeap() };
			pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

			ImGUIEngine::Begin();
			ImGUIEngine::End();
			ImGUIEngine::Draw(pGfxCmdList);
		}
	}

	// Presentation
	{
		CommandQueue* pGfxCmdQueue = CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT);

		m_pSwapChain->PrepareForPresentation(pGfxCmdList);
		pGfxCmdQueue->SubmitToQueue(pGfxCmdList);
		pGfxCmdQueue->ExecuteCommandLists();
		m_pSwapChain->Present();
		pGfxCmdQueue->Signal();
		m_pSwapChain->Swap();
		pGfxCmdQueue->Wait();
	}

	return true;
}
void RendererD3D12::MainRenderPass(CommandList* _pGfxCmdList)
{
	RenderMarker profile(_pGfxCmdList, "MainRenderPass");

	UINT objCBByteSize = CONSTANT_BUFFER_SIZE(sizeof(ObjectCB));

	_pGfxCmdList->SetPipelineState(m_pAlbedoPSO.Get());
	_pGfxCmdList->SetGraphicsRootSignature(m_pAlbedoRS.Get());

	// Per Object Draws
	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		RenderEntity* pModel = m_pRenderEntity[i];

		ID3D12DescriptorHeap* pHeaps[] = { pModel->GetModel()->pSRVHeap->GetHeap(), m_pDescHeapSampler->GetHeap() };
		_pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

		for (UINT i = 0; i < pModel->GetModel()->MeshCount; ++i)
		{
			Mesh& rMesh = pModel->GetModel()->pMeshList[i];

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_ObjectCBs->Resource()->GetGPUVirtualAddress() + (i * objCBByteSize);
			_pGfxCmdList->SetGraphicsRootConstantBufferView(2, objCBAddress);

			if (rMesh.pTexture)
			{
				CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(pModel->GetModel()->pSRVHeap->GetGPUStartHandle());
				texHandle.Offset(rMesh.pTexture->GetHeapIndex(), pModel->GetModel()->pSRVHeap->GetIncrementSize());
				_pGfxCmdList->SetGraphicsRootDescriptorTable(0, texHandle);
			}

			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(m_pDescHeapSampler->GetGPUStartHandle());
			_pGfxCmdList->SetGraphicsRootDescriptorTable(1, samplerHandle);

			auto vbView = rMesh.pVertexBuffer->GetView();
			auto ibView = rMesh.pIndexBuffer->GetView();
			_pGfxCmdList->SetIAVertexBuffers(0, 1, &vbView);
			_pGfxCmdList->SetIAIndexBuffer(&ibView);
			_pGfxCmdList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
		}
	}
}