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
#include "D3D12\Resources\ConstantTable.h"
#include "D3D12\Resources\ConstantBufferResource.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

#include "SysMemory/include/MemoryGlobalTracking.h"

using namespace SysMemory;

#if defined(_DEBUG)
#	define SHADER_CACHE_LOCATION "H:\\Development\\DX12_Project\\_Shaders\\*"
#	define CONTENT_LOCATION "H:\\Development\\DX12_Project\\_Content\\"
#else
#	define SHADER_CACHE_LOCATION "Shaders\\*"
#	define CONTENT_LOCATION "Content\\"
#endif

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
	if (!DeviceD3D12::Instance()->Initialise(CLParser::Instance()->HasArgument("d3ddebug")))
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

struct ModelDefinition
{
	const char* Name = nullptr;
	const char* MaterialName = nullptr;
};

ModelDefinition g_ModelList[] =
{
	//"AnalogMeter.Needle.Dark\\AnalogMeter.fbx",
	{ "Sponza\\Sponza.fbx", "Albedo" }
	//"Cube\\Cube.obj",
};

bool RendererD3D12::LoadContent(void)
{	
	m_bNewModelsLoaded = true;

	LogInfo_Renderer("Loading Models:");
	m_pRenderEntity = new RenderEntity*[_countof(g_ModelList)];
	for (UINT i = 0; i < _countof(g_ModelList); ++i)
	{
		LogInfo_Renderer("\t%s", g_ModelList[i].Name);

		char pModelPath[128];
		snprintf(pModelPath, ARRAYSIZE(pModelPath), "%s\\%s", CONTENT_LOCATION, g_ModelList[i].Name);

		m_pRenderEntity[i] = new RenderEntity();
		m_pRenderEntity[i]->LoadModelFromFile(pModelPath);
		m_pRenderEntity[i]->SetScale(1.0f);
		m_pRenderEntity[i]->SetRotation(0.0f, 0.0f, 0.0f);
		m_pRenderEntity[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_pRenderEntity[i]->SetMaterial(g_ModelList[i].MaterialName);
		m_ModelCount++;
	}

	m_Camera.SetPosition(90.0f, 90.0f, 0.0f);
	//m_Camera.SetPosition(1.5f, 2.5f, 0.0f);

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
		CD3DX12_DESCRIPTOR_RANGE cbvRange(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsDescriptorTable(1, &cbvRange, D3D12_SHADER_VISIBILITY_ALL);

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

	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		m_pRenderEntity[i]->Update();
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
		ImGuiPass(pGfxCmdList);
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
void RendererD3D12::ImGuiPass(CommandList* _pGfxCmdList)
{
#if defined(_DEBUG)
	RenderMarker profile(_pGfxCmdList, "%s", "ImGUI");

	ID3D12DescriptorHeap* pHeaps[] = { m_pImGuiSRVHeap->GetHeap() };
	_pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

	ImGUIEngine::Begin();

	if (ImGui::Begin("Main Camera"))
	{
		float v[3]; 
		v[0] = m_Camera.GetPosition().x;
		v[1] = m_Camera.GetPosition().y;
		v[2] = m_Camera.GetPosition().z;

		if (ImGui::SliderFloat3("Position:", v, -1000.0f, 1000.0f))
		{
			m_Camera.SetPosition(v[0], v[1], v[2]);
		}

		v[0] = m_Camera.GetTarget().x;
		v[1] = m_Camera.GetTarget().y;
		v[2] = m_Camera.GetTarget().z;
		if (ImGui::SliderFloat3("Target:", v, -1000.0f, 1000.0f))
		{
			m_Camera.SetTarget(v[0], v[1], v[2]);
		}

		v[0] = m_Camera.GetUp().x;
		v[1] = m_Camera.GetUp().y;
		v[2] = m_Camera.GetUp().z;
		if (ImGui::SliderFloat3("Up:", v, -1000.0f, 1000.0f))
		{
			m_Camera.SetUp(v[0], v[1], v[2]);
		}
		ImGui::End();
	}

	if (ImGui::Begin("Memory"))
	{
		static const float kMB = 1024 * 1024;
		float fTotalMemUsage = 0.0f;
		int iTotalAllocations = 0;
		for (unsigned int i = 0; i < (unsigned int)MemoryContextCategory::eCategories; ++i)
		{
			const char* pCatName = MemoryGlobalTracking::GetContextName((MemoryContextCategory)i);
			const MemoryContextData& data = MemoryGlobalTracking::GetContextStats((MemoryContextCategory)i);

			int iNetAllocations = data.Allocations - data.Deallocations;
			ImGui::CollapsingHeader(pCatName, ImGuiTreeNodeFlags_Bullet);
			ImGui::Text("Net Allocations: %i", iNetAllocations);
			ImGui::Text("Allocated Size: %0.3f MB", data.TotalAllocationSize / kMB);

			fTotalMemUsage += data.TotalAllocationSize;
			iTotalAllocations += iNetAllocations;
		}
		ImGui::CollapsingHeader("Total Memory Usage:", ImGuiTreeNodeFlags_Bullet);
		ImGui::Text("Size Allocation: %0.3f MB", fTotalMemUsage / kMB);
		ImGui::Text("Net Allocations: %i", iTotalAllocations);
		ImGui::End();
	}

	ImGUIEngine::End();
	ImGUIEngine::Draw(_pGfxCmdList);
#endif
}

void RendererD3D12::MainRenderPass(CommandList* _pGfxCmdList)
{
	RenderMarker profile(_pGfxCmdList, "MainRenderPass");

	_pGfxCmdList->SetPipelineState(m_pAlbedoPSO.Get());
	_pGfxCmdList->SetGraphicsRootSignature(m_pAlbedoRS.Get());

	// Per Object Draws
	UINT objCBByteSize = CONSTANT_BUFFER_SIZE(sizeof(ObjectCB));
	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		RenderEntity* pModel = m_pRenderEntity[i];

		DirectX::XMMATRIX MVP = pModel->GetWorld() * m_Camera.GetView() * m_Camera.GetProjection();
		ConstantTable::Instance()->UpdateValue("ObjectCB", "MVP", &MVP, sizeof(pModel->GetWorld()));

		DescriptorHeap* pDescHeapSrvCbv = DeviceD3D12::Instance()->GetSrvCbvHeap();

		ID3D12DescriptorHeap* pHeaps[] = { pDescHeapSrvCbv->GetHeap(), m_pDescHeapSampler->GetHeap() };
		_pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

		for (UINT i = 0; i < pModel->GetModel()->MeshCount; ++i)
		{
			Mesh& rMesh = pModel->GetModel()->pMeshList[i];
			
			CD3DX12_GPU_DESCRIPTOR_HANDLE cbHandle(pDescHeapSrvCbv->GetGPUStartHandle());
			
			cbHandle.Offset(ConstantTable::Instance()->GetConstantBuffer("ObjectCB")->GetHeapIndex(), pDescHeapSrvCbv->GetIncrementSize());
			_pGfxCmdList->SetGraphicsRootDescriptorTable(2, cbHandle);

			if (rMesh.pTexture)
			{
				CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(pDescHeapSrvCbv->GetGPUStartHandle());
				texHandle.Offset(rMesh.pTexture->GetHeapIndex(), pDescHeapSrvCbv->GetIncrementSize());
				_pGfxCmdList->SetGraphicsRootDescriptorTable(0, texHandle);
			}

			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(m_pDescHeapSampler->GetGPUStartHandle());
			_pGfxCmdList->SetGraphicsRootDescriptorTable(1, samplerHandle);

			if (DeviceD3D12::Instance()->FlushState())
			{
				auto vbView = rMesh.pVertexBuffer->GetView();
				auto ibView = rMesh.pIndexBuffer->GetView();
				_pGfxCmdList->SetIAVertexBuffers(0, 1, &vbView);
				_pGfxCmdList->SetIAIndexBuffer(&ibView);
				_pGfxCmdList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
			}
		}
	}
}