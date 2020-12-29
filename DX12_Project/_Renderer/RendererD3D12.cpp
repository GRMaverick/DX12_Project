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

	ShaderCache::Instance()->Load(SHADER_CACHE_LOCATION);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY);

	if (!DeviceD3D12::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pImGuiSRVHeap, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ImGUI SRV"))
		return false;

	ImGUIEngine::Initialise(_pWindow->GetWindowHandle(), &m_pImGuiSRVHeap);

	if (!DeviceD3D12::Instance()->CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, L"Swap Chain"))
		return false;
	
	if (!LoadContent())
		return false;

	return true;
}

struct ModelDefinition
{
	const char* Name = nullptr;
	const char* MaterialName = nullptr;
	float		Position[3] = { 0 };
};

#define SPONZA
//#define CUBES

ModelDefinition g_ModelList[] =
{
	//"AnalogMeter.Needle.Dark\\AnalogMeter.fbx",
#if defined(SPONZA)
	{ "Sponza\\Sponza.fbx", "Albedo" }
#endif
#if defined(CUBES)
	{ "Cube\\Cube.obj", "Albedo", {0.0f, 0.0f, 0.0f} },
	{ "Cube\\Cube.obj", "Albedo", {2.0f, 0.0f, 0.0f} },
	{ "Cube\\Cube.obj", "Albedo", {-2.0f, 0.0f, 0.0f} },
#endif
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
		m_pRenderEntity[i]->SetPosition(g_ModelList[i].Position[0], g_ModelList[i].Position[1], g_ModelList[i].Position[2]);
		m_pRenderEntity[i]->SetMaterial(g_ModelList[i].MaterialName);
		m_pRenderEntity[i]->SetConstantBuffer(ConstantTable::Instance()->CreateConstantBuffer("ObjectCB"));
		m_ModelCount++;
	}

#if defined(SPONZA)
	m_Camera.SetPosition(90.0f, 90.0f, 0.0f);
#endif

#if defined(CUBES)
	m_Camera.SetPosition(0.0f, 1.5f, -6.0f);
#endif

	m_Camera.SetUp(0.0f, 1.0f, 0.0f);
	m_Camera.SetTarget(0.0f, 0.0f, 0.0f);
	m_Camera.SetFieldOfView(45.0f);
	m_Camera.SetAspectRatio(1920.0f / 1080.0f);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandLists();
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->Flush();

	// Default Sampler
	m_DefaultSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	m_DefaultSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_DefaultSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_DefaultSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_DefaultSampler.MinLOD = 0;
	m_DefaultSampler.MaxLOD = D3D12_FLOAT32_MAX;
	m_DefaultSampler.MipLODBias = 0.0f;
	m_DefaultSampler.MaxAnisotropy = 1;
	m_DefaultSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return true;
}

void RendererD3D12::Update(double _deltaTime)
{
	m_Camera.Update();

	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		m_pRenderEntity[i]->Update();
	}
}

void RendererD3D12::UpdatePassConstants()
{
}

bool RendererD3D12::Render(void)
{
	DeviceD3D12::Instance()->BeginFrame();

	CommandList* pGfxCmdList = DeviceD3D12::Instance()->GetImmediateContext();

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

	DeviceD3D12::Instance()->EndFrame();

	return true;
}

void RendererD3D12::ImGuiPass(CommandList* _pGfxCmdList)
{
#if defined(_DEBUG)
	RenderMarker profile(_pGfxCmdList, "%s", "ImGUI");

	ID3D12DescriptorHeap* pHeaps[] = { m_pImGuiSRVHeap.GetHeap() };
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

	DeviceD3D12* pDevice = DeviceD3D12::Instance();
	ConstantTable* pConstantTable = ConstantTable::Instance();

	// Per Pass Setup
	if (!m_pMainPassCB)
	{
		m_pMainPassCB = pConstantTable->CreateConstantBuffer("PassCB");
	}

	DirectX::XMMATRIX viewProjection = m_Camera.GetView() * m_Camera.GetProjection();
	m_pMainPassCB->UpdateValue("ViewProjection", &viewProjection, sizeof(DirectX::XMMATRIX));

	// Per Object Draws
	for (UINT i = 0; i < m_ModelCount; ++i)
	{
		RenderEntity* pModel = m_pRenderEntity[i];
		DirectX::XMMATRIX world = pModel->GetWorld();
		ConstantBufferResource* pModelCB = pModel->GetConstantBuffer();

		pDevice->SetMaterial(pModel->GetMaterialName());
		pDevice->SetSamplerState("Albedo", m_DefaultSampler);

		pModelCB->UpdateValue("World", &world, sizeof(DirectX::XMMATRIX));

		pDevice->SetConstantBuffer("ObjectCB", pModelCB);
		pDevice->SetConstantBuffer("PassCB", m_pMainPassCB);

		for (UINT i = 0; i < pModel->GetModel()->MeshCount; ++i)
		{
			Mesh& rMesh = pModel->GetModel()->pMeshList[i];

			pDevice->SetTexture("Albedo", (Texture2DResource*)rMesh.pTexture);
			if (DeviceD3D12::Instance()->FlushState())
			{
				auto vbView = ((VertexBufferResource*)rMesh.pVertexBuffer)->GetView();
				auto ibView = ((IndexBufferResource*)rMesh.pIndexBuffer)->GetView();
				_pGfxCmdList->SetIAVertexBuffers(0, 1, &vbView);
				_pGfxCmdList->SetIAIndexBuffer(&ibView);
				_pGfxCmdList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
			}
		}
	}
}