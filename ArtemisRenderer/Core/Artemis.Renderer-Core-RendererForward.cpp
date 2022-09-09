module;

#include "RendererForward.h"

#include <assert.h>
#include <DirectXMath.h>

#include "Helpers/AssimpLoader.h"
#include "Helpers/d3dx12.h"
#include "Helpers/Defines.h"

#include <WICTextureLoader.h>

//#include "SysCore\_Window\GameWindow.h"

//#include "SysMemory\include\MemoryGlobalTracking.h"

//#include "SysUtilities\_Loaders\CLParser.h"
//#include "SysUtilities\_Profiling\ProfileMarker.h"

#include "Scene/Camera.h"
#include "Scene/RenderEntity.h"

//#include "_ImGUI\ImGUIEngine.h"

#include "Device/SwapChain.h"
#include "Device/CommandList.h"
#include "Device/CommandQueue.h"
#include "Device/RenderDevice.h"

#include "Shaders/ShaderCache.h"

#include "Resources/ConstantTable.h"
#include "Resources/DescriptorHeap.h"
#include "Resources/Texture2D.h"
#include "Resources/ConstantBuffer.h"
#include "Resources/CBStructures.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

module Artemis.Renderer:Core;

using namespace ArtemisRenderer::Device;
using namespace ArtemisRenderer::Resources;
using namespace ArtemisRenderer::Shaders;
using namespace ArtemisRenderer::Helpers;
using namespace ArtemisRenderer::Scene;

using namespace DirectX;

#if defined(_DEBUG)
#	define SHADER_CACHE_LOCATION "Shaders\\*"
#	define CONTENT_LOCATION "Content\\"
#else
#	define SHADER_CACHE_LOCATION "Shaders\\*"
#	define CONTENT_LOCATION "Content\\"
#endif

namespace ArtemisRenderer::Core
{
	Renderer::Renderer(void)
	{
		m_pSwapChain = nullptr;
	}

	Renderer::~Renderer(void)
	{
		if (m_pSwapChain) delete m_pSwapChain; m_pSwapChain = nullptr;
	}

	bool Renderer::Initialise(SysCore::GameWindow* _pWindow)
	{
		//if (!RenderDevice::Instance()->Initialise(CLParser::Instance()->HasArgument("d3ddebug")))
		//	return false;

		ShaderCache::Instance()->Load(SHADER_CACHE_LOCATION);

		CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT);
		CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY);

		if (!RenderDevice::Instance()->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pImGuiSRVHeap, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ImGUI SRV"))
			return false;

		//ImGUIEngine::Initialise(_pWindow->GetWindowHandle(), m_pImGuiSRVHeap);

		if (!RenderDevice::Instance()->CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, L"Swap Chain"))
			return false;

		if (!LoadContent())
			return false;

		return true;
	}

	struct ModelDefinition
	{
		const char* MeshFilename = nullptr;
		const char* ObjectName = nullptr;
		const char* MaterialName = nullptr;
		float		Position[3] = { 0 };
		float		Scale = 1.0f;
	};

#define SPONZA
	//#define CUBES

	ModelDefinition g_ModelList[] =
	{
	#if defined(SPONZA)
		{ "Sponza\\Sponza.fbx", "Scene", "AlbedoPhongNormal",{0.0f, 0.0f, 0.0f}, 1.0f }
	#endif
	#if defined(CUBES)
		{ "Cube\\Cube.obj", "Cube_Albedo", "Albedo", {0.0f, 0.0f, -1.25f}, 1.0f },
		{ "StorageContainer\\Cube.obj", "Storage_Phong", "AlbedoPhong", {1.25f, 0.0f, 0.0f}, 1.0f },
		{ "StorageContainer\\Cube.obj", "Storage_Normal", "AlbedoPhongNormal", {-1.25f, 0.0f, 0.0f}, 1.0f },
	#endif
	};

	bool Renderer::LoadContent(void)
	{
		m_bNewModelsLoaded = true;

		m_pMainPassCB = (ConstantBuffer*)ConstantTable::Instance()->CreateConstantBuffer("PassCB");
		m_pLightsCB = (ConstantBuffer*)ConstantTable::Instance()->CreateConstantBuffer("LightCB");
		m_pSpotlightCB = (ConstantBuffer*)ConstantTable::Instance()->CreateConstantBuffer("SpotlightCB");

		Material material;
		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material.Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
		material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		LogInfo("Loading Models:");
		m_pRenderEntity = new RenderEntity * [_countof(g_ModelList)];
		for (UINT i = 0; i < _countof(g_ModelList); ++i)
		{
			LogInfo("\t%s", g_ModelList[i].MeshFilename);

			char pModelPath[128];
			snprintf(pModelPath, ARRAYSIZE(pModelPath), "%s\\%s", CONTENT_LOCATION, g_ModelList[i].MeshFilename);

			m_pRenderEntity[i] = new RenderEntity();
			m_pRenderEntity[i]->LoadModelFromFile(pModelPath);
			m_pRenderEntity[i]->SetScale(g_ModelList[i].Scale);
			m_pRenderEntity[i]->SetRotation(0.0f, 0.0f, 0.0f);
			m_pRenderEntity[i]->SetPosition(g_ModelList[i].Position[0], g_ModelList[i].Position[1], g_ModelList[i].Position[2]);
			m_pRenderEntity[i]->SetMaterial(g_ModelList[i].MaterialName);
			m_pRenderEntity[i]->SetMaterialData(material);
			m_pRenderEntity[i]->SetConstantBuffer(ConstantTable::Instance()->CreateConstantBuffer("ObjectCB"));
			m_ModelCount++;
		}

		m_Camera = new Camera();
		m_Light = new Light();

#if defined(SPONZA)
		m_Camera->SetPosition(180.0f, 180.0f, 0.0f);
		m_Camera->SetTarget(0.0f, 180.0f, 0.0f);
		m_Light->Position = XMFLOAT3(78.0f, 43.0f, 0.0f);
#endif

#if defined(CUBES)
		m_Camera->SetPosition(0.0f, 5.0f, 5.0f);
		m_Camera->SetTarget(0.0f, 0.0f, 0.0f);

		m_Light->Position = XMFLOAT3(-0.265f, 0.265f, -1.053f);
#endif

		m_Camera->SetUp(0.0f, 1.0f, 0.0f);
		m_Camera->SetFieldOfView(45.0f);
		m_Camera->SetAspectRatio(1920.0f / 1080.0f);

		m_Light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_Light->SpecularPower = 0.0f;

		CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandLists();
		CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->Flush();

		return true;
	}

	void Renderer::Update(double _deltaTime)
	{
		UpdatePassConstants();

		for (UINT i = 0; i < m_ModelCount; ++i)
		{
			m_pRenderEntity[i]->Update();

			Object obj;
			obj.World = m_pRenderEntity[i]->GetWorld();
			obj.Material = m_pRenderEntity[i]->GetMaterialData();

			if (m_pRenderEntity[i]->GetConstantBuffer())
				m_pRenderEntity[i]->GetConstantBuffer()->UpdateValue("World", &obj, sizeof(Object));
		}
	}

	void Renderer::UpdatePassConstants()
	{
		m_Camera->Update();

		Pass cbPass;
		cbPass.EyePosition = m_Camera->GetPosition();
		cbPass.ViewProjection = m_Camera->GetView() * m_Camera->GetProjection();

		if (m_pMainPassCB)
			m_pMainPassCB->UpdateValue(nullptr, &cbPass, sizeof(Pass));

		if (m_pLightsCB)
			m_pLightsCB->UpdateValue(nullptr, m_Light, sizeof(Light));

		if (m_pSpotlightCB)
			m_pSpotlightCB->UpdateValue(nullptr, m_Spotlight, sizeof(Spotlight));
	}

	bool Renderer::Render(void)
	{
		RenderDevice::Instance()->BeginFrame();

		CommandList* pGfxCmdList = RenderDevice::Instance()->GetImmediateContext();
		pGfxCmdList->Reset();

		// Rendering
		{
			//RenderMarker profile(pGfxCmdList, "Render");

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

		RenderDevice::Instance()->EndFrame();

		return true;
	}

	void Renderer::ImGuiPass(CommandList* _pGfxCmdList)
	{
#if 0 //defined(_DEBUG)
		//RenderMarker profile(_pGfxCmdList, "%s", "ImGUI");

		ID3D12DescriptorHeap* pHeaps[] = { m_pImGuiSRVHeap->GetHeap() };
		_pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));

		ImGUIEngine::Begin();

		if (ImGui::Begin("Main Camera"))
		{
			float v[3];
			v[0] = m_Camera->GetPosition().x;
			v[1] = m_Camera->GetPosition().y;
			v[2] = m_Camera->GetPosition().z;

			if (ImGui::SliderFloat3("Position:", v, -1000.0f, 1000.0f))
			{
				m_Camera->SetPosition(v[0], v[1], v[2]);
			}

			v[0] = m_Camera->GetTarget().x;
			v[1] = m_Camera->GetTarget().y;
			v[2] = m_Camera->GetTarget().z;
			if (ImGui::SliderFloat3("Target:", v, -1000.0f, 1000.0f))
			{
				m_Camera->SetTarget(v[0], v[1], v[2]);
			}

			v[0] = m_Camera->GetUp().x;
			v[1] = m_Camera->GetUp().y;
			v[2] = m_Camera->GetUp().z;
			if (ImGui::SliderFloat3("Up:", v, -1000.0f, 1000.0f))
			{
				m_Camera->SetUp(v[0], v[1], v[2]);
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

		if (ImGui::Begin("Lights"))
		{
			float v[3];
			v[0] = m_Light->Position.x; v[1] = m_Light->Position.y; v[2] = m_Light->Position.z;
			if (ImGui::SliderFloat3("Position:", v, -180.0f, 180.0f))
			{
				m_Light->Position.x = v[0]; m_Light->Position.y = v[1]; m_Light->Position.z = v[2];
			}

			v[0] = m_Light->Diffuse.x; v[1] = m_Light->Diffuse.y; v[2] = m_Light->Diffuse.z;
			if (ImGui::SliderFloat3("Diffuse:", v, 0.0f, 1.0f))
			{
				m_Light->Diffuse.x = v[0]; m_Light->Diffuse.y = v[1]; m_Light->Diffuse.z = v[2];
			}

			v[0] = m_Light->Ambient.x; v[1] = m_Light->Ambient.y; v[2] = m_Light->Ambient.z;
			if (ImGui::SliderFloat3("Ambient:", v, 0.0f, 1.0f))
			{
				m_Light->Ambient.x = v[0]; m_Light->Ambient.y = v[1]; m_Light->Ambient.z = v[2];
			}

			v[0] = m_Light->Specular.x; v[1] = m_Light->Specular.y; v[2] = m_Light->Specular.z;
			if (ImGui::SliderFloat3("Specular:", v, 0.0f, 1.0f))
			{
				m_Light->Specular.x = v[0]; m_Light->Specular.y = v[1]; m_Light->Specular.z = v[2];
			}

			float nS = m_Light->SpecularPower;
			if (ImGui::SliderFloat("Specular Power:", &nS, 0.0f, 10.0f))
			{
				m_Light->SpecularPower = nS;
			}

			ImGui::End();
		}

		if (ImGui::Begin("Objects:"))
		{
			float v[3];
			for (unsigned int i = 0; i < m_ModelCount; ++i)
			{
				if (ImGui::CollapsingHeader(g_ModelList[i].ObjectName))
				{
					Material material = m_pRenderEntity[i]->GetMaterialData();

					v[0] = material.Diffuse.x; v[1] = material.Diffuse.y; v[2] = material.Diffuse.z;
					if (ImGui::SliderFloat3("Diffuse:", v, 0.0f, 1.0f))
					{
						material.Diffuse.x = v[0]; material.Diffuse.y = v[1]; material.Diffuse.z = v[2];
					}

					v[0] = material.Ambient.x; v[1] = material.Ambient.y; v[2] = material.Ambient.z;
					if (ImGui::SliderFloat3("Ambient:", v, 0.0f, 1.0f))
					{
						material.Ambient.x = v[0]; material.Ambient.y = v[1]; material.Ambient.z = v[2];
					}

					v[0] = material.Specular.x; v[1] = material.Specular.y; v[2] = material.Specular.z;
					if (ImGui::SliderFloat3("Specular:", v, 0.0f, 1.0f))
					{
						material.Specular.x = v[0]; material.Specular.y = v[1]; material.Specular.z = v[2];
					}

					m_pRenderEntity[i]->SetMaterialData(material);
				}
			}
			ImGui::End();
		}

		ImGUIEngine::End();
		ImGUIEngine::Draw(_pGfxCmdList);
#endif
	}

	void Renderer::MainRenderPass(CommandList* _pGfxCmdList)
	{
		//RenderMarker profile(_pGfxCmdList, "MainRenderPass");

		RenderDevice* pDevice = RenderDevice::Instance();
		ConstantTable* pConstantTable = ConstantTable::Instance();

		// Per Object Draws
		for (UINT i = 0; i < m_ModelCount; ++i)
		{
			RenderEntity* pModel = m_pRenderEntity[i];
			DirectX::XMMATRIX world = pModel->GetWorld();
			ConstantBuffer* pModelCB = pModel->GetConstantBuffer();

			pDevice->SetMaterial(pModel->GetMaterialName());
			pDevice->SetSamplerState("Albedo", pDevice->GetDefaultSamplerState());
			pDevice->SetSamplerState("Normal", pDevice->GetDefaultSamplerState());

			pDevice->SetConstantBuffer("ObjectCB", pModelCB);
			pDevice->SetConstantBuffer("PassCB", m_pMainPassCB);
			pDevice->SetConstantBuffer("LightCB", m_pLightsCB);
			pDevice->SetConstantBuffer("SpotlightCB", m_pSpotlightCB);

			for (UINT i = 0; i < pModel->GetModel()->MeshCount; ++i)
			{
				Mesh& rMesh = pModel->GetModel()->pMeshList[i];

				pDevice->SetTexture("Albedo", (Texture2D*)rMesh.pTexture[ALBEDO]);
				pDevice->SetTexture("Normal", (Texture2D*)rMesh.pTexture[NORMAL]);

				if (pDevice->FlushState())
				{
					auto vbView = ((VertexBuffer*)rMesh.pVertexBuffer)->GetView();
					auto ibView = ((IndexBuffer*)rMesh.pIndexBuffer)->GetView();
					_pGfxCmdList->SetIAVertexBuffers(0, 1, &vbView);
					_pGfxCmdList->SetIAIndexBuffer(&ibView);
					_pGfxCmdList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
				}
			}
		}
	}
}