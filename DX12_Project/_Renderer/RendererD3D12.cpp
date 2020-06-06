#include "Defines.h"

#include "CoreWindow.h"
#include "RendererD3D12.h"
#include "AssimpLoader.h"

#include "d3dx12.h"

#include <assert.h>
#include <DirectXMath.h>

#include <WICTextureLoader.h>

#include "Scene\RenderEntity.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

PRAGMA_TODO("VSync Support")
PRAGMA_TODO("Test Interleaved Vertex Format v Packed Vertex Format")
PRAGMA_TODO("Memory Profiling")
PRAGMA_TODO("Integrate ImGUI")
PRAGMA_TODO("Integrate ASSIMP")
PRAGMA_TODO("Implement Logger")
PRAGMA_TODO("Command Line Parser")
PRAGMA_TODO("Data Driven Pipelines")
PRAGMA_TODO("Constant Buffers, CBVs and Descriptor Tables")
PRAGMA_TODO("Scene Configuration File")

#define SHADER_CACHE_LOCATION "C:\\Users\\Maverick\\Source\\Repos\\DX12_Project\\DX12_Project\\_Shaders\\*"

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

RendererD3D12::RendererD3D12(void)
{
	m_pSwapChain = nullptr;
	m_pGFXCommandList = nullptr;
	m_pGFXCommandQueue = nullptr;
	m_pCopyCommandList = nullptr;
	m_pCopyCommandQueue = nullptr;
}

RendererD3D12::~RendererD3D12(void)
{
	if (m_pSwapChain) delete m_pSwapChain; m_pSwapChain = nullptr;
	if (m_pGFXCommandList) delete m_pGFXCommandList; m_pGFXCommandList = nullptr;
	if (m_pGFXCommandQueue) delete m_pGFXCommandQueue; m_pGFXCommandQueue = nullptr;
	if (m_pCopyCommandList) delete m_pCopyCommandList; m_pCopyCommandList = nullptr;
	if (m_pCopyCommandQueue) delete m_pCopyCommandQueue; m_pCopyCommandQueue = nullptr;
}

bool RendererD3D12::Initialise(CoreWindow* _pWindow)
{
	if (!DeviceD3D12::Instance()->Initialise(true))
		return false;

	m_ShaderCache = ShaderCache(SHADER_CACHE_LOCATION);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_DIRECT);
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY);

	if (!DeviceD3D12::Instance()->CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_pGFXCommandList, L"GFX"))
		return false;

	if (!DeviceD3D12::Instance()->CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, &m_pCopyCommandList, L"CPY"))
		return false;

	if (!DeviceD3D12::Instance()->CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, m_pGFXCommandQueue, L"Swap Chain"))
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
	//"Content\\S&W_45ACP\\Handgun_fbx_7.4_binary.fbx",
	//"Content\\Room\\OBJ\\Room.obj",
	//"Content\\Cube\\Cube.obj",
};

const wchar_t* g_TexList[] =
{
	L"Content\\Cube\\AnotherCrate.dds",
};

bool RendererD3D12::LoadContent(void)
{	
	m_pRenderEntity = new RenderEntity();
	m_pRenderEntity->LoadModelFromFile(g_ModelList[0]);
	m_pRenderEntity->SetScale(1.0f);
	m_pRenderEntity->SetRotation(0.0f, 0.0f, 0.0f);
	m_pRenderEntity->SetPosition(0.0f, 0.0f, -1.0f);

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandLists();
	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->Flush();

	return true;
}

bool RendererD3D12::CreatePipelineState(void)
{
	// Basic Colour PSO
	{
		CD3DX12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		if (!DeviceD3D12::Instance()->CreateRootSignature(rootParameters, _countof(rootParameters), m_pBasicRS.GetAddressOf()))
			return false;

		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		PipelineStateDesc psDesc;
		ZeroMemory(&psDesc, sizeof(PipelineStateDesc));
		psDesc.VertexShader = m_ShaderCache.GetShader("BasicVS.vs");
		psDesc.PixelShader = m_ShaderCache.GetShader("BasicPS.ps");
		psDesc.InputLayout = { inputLayout, _countof(inputLayout) };
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

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

		if (!DeviceD3D12::Instance()->CreateRootSignature(rootParameters, _countof(rootParameters), m_pAlbedoRS.GetAddressOf()))
			return false;

		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		PipelineStateDesc psDesc;
		ZeroMemory(&psDesc, sizeof(PipelineStateDesc));
		psDesc.VertexShader = m_ShaderCache.GetShader("AlbedoVS.vs");
		psDesc.PixelShader = m_ShaderCache.GetShader("AlbedoPS.ps");
		psDesc.InputLayout = { inputLayout, _countof(inputLayout) };
		psDesc.RootSignature = m_pAlbedoRS.Get();

		if (!DeviceD3D12::Instance()->CreatePipelineState(psDesc, m_pAlbedoPSO.GetAddressOf()))
			return false;
	}

	return true;
}
void RendererD3D12::Update(float _deltaTime)
{
	m_pRenderEntity->Update();
	
	m_Camera.SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera.SetUp(0.0f, 1.0f, 0.0f);
	m_Camera.SetTarget(0.0f, 0.0f, 0.0f);
	m_Camera.SetFieldOfView(45.0f);
	m_Camera.SetAspectRatio(1920.0f / 1080.0f);
	m_Camera.Update();
}
bool RendererD3D12::Render(void)
{
	CommandList* pGfxCmdList = CommandList::Build(D3D12_COMMAND_LIST_TYPE_DIRECT);

	// Rendering
	{
		m_pSwapChain->PrepareForRendering(pGfxCmdList);
		m_pSwapChain->SetOMRenderTargets(pGfxCmdList);
		pGfxCmdList->SetPipelineState(m_pAlbedoPSO.Get());
		pGfxCmdList->SetGraphicsRootSignature(m_pAlbedoRS.Get());
		pGfxCmdList->SetIAPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Per Object Draws
		ID3D12DescriptorHeap* pHeaps[] = { m_pRenderEntity->GetModel()->pSRVHeap->GetHeap(), m_pDescHeapSampler->GetHeap() };
		pGfxCmdList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));
		for (UINT i = 0; i < m_pRenderEntity->GetModel()->MeshCount; ++i)
		{
			Mesh& rMesh = m_pRenderEntity->GetModel()->pMeshList[i];

			XMMATRIX mvpMatrix = XMMatrixMultiply(m_pRenderEntity->GetWorld(), m_Camera.GetView());
			mvpMatrix = XMMatrixMultiply(mvpMatrix, m_Camera.GetProjection());
			pGfxCmdList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

			if (rMesh.pTexture)
			{
				CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_pRenderEntity->GetModel()->pSRVHeap->GetGPUStartHandle());
				texHandle.Offset(rMesh.pTexture->GetHeapIndex(), m_pRenderEntity->GetModel()->pSRVHeap->GetIncrementSize());
				pGfxCmdList->SetGraphicsRootDescriptorTable(1, texHandle);
			}

			CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(m_pDescHeapSampler->GetGPUStartHandle());
			pGfxCmdList->SetGraphicsRootDescriptorTable(2, samplerHandle);

			pGfxCmdList->SetIAVertexBuffers(0, 1, &rMesh.pVertexBuffer->GetView());
			pGfxCmdList->SetIAIndexBuffer(&rMesh.pIndexBuffer->GetView());
			pGfxCmdList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
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