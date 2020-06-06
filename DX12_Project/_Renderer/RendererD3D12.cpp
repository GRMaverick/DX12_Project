#include "Defines.h"

#include "CoreWindow.h"
#include "RendererD3D12.h"
#include "AssimpLoader.h"

#include "d3dx12.h"

#include <assert.h>
#include <DirectXMath.h>

#include <WICTextureLoader.h>


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

#define SHADER_CACHE_LOCATION "C:\\Users\\Maverick\\Source\\Repos\\DX12_Project\\DX12_Project\\_Shaders\\*"

struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

static VertexPosColor g_Vertices[8] = {
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
};

static DWORD g_Indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7
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
	if (!m_Device.Initialise(true))
		return false;

	m_ShaderCache = ShaderCache(SHADER_CACHE_LOCATION);

	if (!m_Device.CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_pGFXCommandQueue))
		return false;
	if (!m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_pGFXCommandList, L"GFX"))
		return false;

	if (!m_Device.CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY, &m_pCopyCommandQueue))
		return false;
	if (!m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, &m_pCopyCommandList, L"CPY"))
		return false;

	if (!m_Device.CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, m_pGFXCommandQueue))
		return false;
	
	if (!LoadContent())
		return false;

	if (!CreatePipelineState())
		return false;

	m_ModelMatrix = XMMatrixIdentity();
	m_ModelMatrix = XMMatrixTranslation(0.0f, 0.0f, - 1.0f);
	m_ViewMatrix = XMMatrixIdentity();
	m_ProjectionMatrix = XMMatrixIdentity();

	return true;
}

const char* g_ModelList[] =
{
	//"Content\\AnalogMeter.Needle.Dark\\AnalogMeter.fbx",
	//"Content\\S&W_45ACP\\Handgun_fbx_7.4_binary.fbx",
	//"Content\\Room\\Room.c4d",
	"Content\\Cube\\Cube.obj",
};
const wchar_t* g_TextureList[]
{
	L"Content\\Cube\\WoodCrate01.dds",
};

bool RendererD3D12::LoadContent(void)
{	
	m_pCopyCommandList->Reset();
	
	if (!AssimpLoader::LoadModel(&m_Device, m_pCopyCommandList, g_ModelList[0], &m_pModel))
		return false;

	//if (!AssimpLoader::LoadModel(&m_Device, m_pCopyCommandList, "Content\\Cube\\Cube.obj", &m_pDialModel))
	//	return false;

	if (!m_Device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pDescHeapSRV, _countof(g_TextureList), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		return true;

	// Load Texture
	{	
		if (!m_Device.CreateTexture2D(g_TextureList[0], m_pCopyCommandList, &m_pModel->pMeshList[0].pTexture, m_pDescHeapSRV, g_TextureList[0]))
			return false;
		delete m_pModel->pSRVHeap;
		m_pModel->pSRVHeap = m_pDescHeapSRV;
	}

	m_pCopyCommandQueue->ExecuteCommandLists(m_pCopyCommandList, 1);
	m_pCopyCommandQueue->Flush();
		
	return true;
}

bool RendererD3D12::CreatePipelineState(void)
{
	// Basic Colour PSO
	{
		CD3DX12_ROOT_PARAMETER rootParameters[1];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		if (!m_Device.CreateRootSignature(rootParameters, _countof(rootParameters), m_pBasicRS.GetAddressOf()))
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

		if (!m_Device.CreatePipelineState(psDesc, m_pBasicPSO.GetAddressOf()))
			return false;
	}

	// Albedo PSO
	{
		if (!m_Device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_pDescHeapSampler, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
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
		if (!m_Device.CreateSamplerState(&samplerDesc, m_pDescHeapSampler->GetCPUStartHandle()))
			return false;

		CD3DX12_DESCRIPTOR_RANGE srvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		CD3DX12_DESCRIPTOR_RANGE samplerRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
		rootParameters[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[2].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);

		if (!m_Device.CreateRootSignature(rootParameters, _countof(rootParameters), m_pAlbedoRS.GetAddressOf()))
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

		if (!m_Device.CreatePipelineState(psDesc, m_pAlbedoPSO.GetAddressOf()))
			return false;
	}

	return true;
}
void RendererD3D12::Update(float _deltaTime)
{
	PRAGMA_TODO("Refactor into Object class")

	float sRotationSpeed = 25.0f * _deltaTime;
	m_CurrentRotation += sRotationSpeed;

	// Update the model matrix. 
	float angle = static_cast<float>(m_CurrentRotation);
	const XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);
	m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

	PRAGMA_TODO("Refactor into Camera Class")
	// Update the view matrix.
	const XMVECTOR eyePosition = XMVectorSet(0, 0, -5, 1);
	const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	m_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

	// Update the projection matrix.
	m_FieldOfView = 45.0;
	m_AspectRatio = 1024 / 768; //static_cast<float>(GetClientWidth()) / static_cast<float>(GetClientHeight());
	m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FieldOfView), m_AspectRatio, 0.1f, 100.0f);
}
bool RendererD3D12::Render(void)
{
	m_pGFXCommandList->Reset();
	
	// Rendering
	{
		m_pSwapChain->PrepareForRendering(m_pGFXCommandList);
		m_pSwapChain->SetOMRenderTargets(m_pGFXCommandList);
		m_pGFXCommandList->SetPipelineState(m_pAlbedoPSO.Get());
		m_pGFXCommandList->SetGraphicsRootSignature(m_pAlbedoRS.Get());
		m_pGFXCommandList->SetIAPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Per Object Draws
		{
			ID3D12DescriptorHeap* pHeaps[] = { m_pModel->pSRVHeap->GetHeap(), m_pDescHeapSampler->GetHeap() };
			m_pGFXCommandList->SetDescriptorHeaps(pHeaps, _countof(pHeaps));
			for (UINT i = 0; i < m_pModel->MeshCount; ++i)
			{
				Mesh& rMesh = m_pModel->pMeshList[i];

				PRAGMA_TODO("Constant Buffer objects");
				XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
				mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
				m_pGFXCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

				if (rMesh.pTexture)
				{
					CD3DX12_GPU_DESCRIPTOR_HANDLE texHandle(m_pModel->pSRVHeap->GetGPUStartHandle());
					texHandle.Offset(rMesh.pTexture->GetHeapIndex(), m_pModel->pSRVHeap->GetIncrementSize());
					m_pGFXCommandList->SetGraphicsRootDescriptorTable(1, texHandle);
				}

				CD3DX12_GPU_DESCRIPTOR_HANDLE samplerHandle(m_pDescHeapSampler->GetGPUStartHandle());
				m_pGFXCommandList->SetGraphicsRootDescriptorTable(2, samplerHandle);

				m_pGFXCommandList->SetIAVertexBuffers(0, 1, &rMesh.pVertexBuffer->GetView());
				m_pGFXCommandList->SetIAIndexBuffer(&rMesh.pIndexBuffer->GetView());
				m_pGFXCommandList->DrawIndexedInstanced(rMesh.Indices, 1, 0, 0, 0);
			}
		}
	}

	// Presentation
	{
		m_pSwapChain->PrepareForPresentation(m_pGFXCommandList);
		m_pGFXCommandQueue->ExecuteCommandLists(m_pGFXCommandList, 1);
		m_pSwapChain->Present();
		m_pGFXCommandQueue->Signal();
		m_pSwapChain->Swap();
		m_pGFXCommandQueue->Wait();
	}

	return true;
}