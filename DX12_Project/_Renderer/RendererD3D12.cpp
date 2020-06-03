#include "Defines.h"

#include "CoreWindow.h"
#include "RendererD3D12.h"

#include "d3dx12.h"

#include <assert.h>
#include <DirectXMath.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

using namespace Microsoft::WRL;
using namespace DirectX;

PRAGMA_TODO("VSync Support")
PRAGMA_TODO("Test Interleaved Vertex Format v Packed Vertex Format")
PRAGMA_TODO("PIX Profile Markers")
PRAGMA_TODO("Integrate ImGUI")

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
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}
RendererD3D12::~RendererD3D12(void)
{
	if (m_pSwapChain) delete m_pSwapChain; m_pSwapChain = nullptr;
	if (m_pGFXCommandList) delete m_pGFXCommandList; m_pGFXCommandList = nullptr;
	if (m_pGFXCommandQueue) delete m_pGFXCommandQueue; m_pGFXCommandQueue = nullptr;
	if (m_pCopyCommandList) delete m_pCopyCommandList; m_pCopyCommandList = nullptr;
	if (m_pCopyCommandQueue) delete m_pCopyCommandQueue; m_pCopyCommandQueue = nullptr;
	if (m_pVertexBuffer) delete m_pVertexBuffer; m_pVertexBuffer = nullptr;
	if (m_pIndexBuffer) delete m_pIndexBuffer; m_pIndexBuffer = nullptr;
}

bool RendererD3D12::Initialise(CoreWindow* _pWindow)
{
	if (!m_Device.Initialise(true))
		return false;

	m_ShaderCache = ShaderCache(SHADER_CACHE_LOCATION);

	if (!m_Device.CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_pGFXCommandQueue))
		return false;
	if (!m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, &m_pGFXCommandList))
		return false;

	if (!m_Device.CreateCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY, &m_pCopyCommandQueue))
		return false;
	if (!m_Device.CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, &m_pCopyCommandList))
		return false;

	if (!m_Device.CreateSwapChain(&m_pSwapChain, _pWindow, BACK_BUFFERS, m_pGFXCommandQueue))
		return false;
	
	if (!LoadCube())
		return false;

	if (!CreatePipelineState())
		return false;

	m_ModelMatrix = XMMatrixIdentity();
	m_ModelMatrix = XMMatrixTranslation(0.0f, 0.0f, - 1.0f);
	m_ViewMatrix = XMMatrixIdentity();
	m_ProjectionMatrix = XMMatrixIdentity();

	return true;
}

bool RendererD3D12::LoadCube(void)
{	
	m_pCopyCommandList->Reset();

	if (!m_Device.CreateVertexBufferResource(m_pCopyCommandList, _countof(g_Vertices), sizeof(VertexPosColor), D3D12_RESOURCE_FLAG_NONE, (void*)g_Vertices, &m_pVertexBuffer))
		return false;

	if (!m_Device.CreateIndexBufferResource(m_pCopyCommandList, _countof(g_Indicies), sizeof(DWORD), D3D12_RESOURCE_FLAG_NONE, (void*)g_Indicies, &m_pIndexBuffer))
		return false;

	m_pCopyCommandQueue->ExecuteCommandLists(m_pCopyCommandList, 1);
	m_pCopyCommandQueue->Flush();

	return true;
}

bool RendererD3D12::CreatePipelineState(void)
{
	// Create Root Sig (once?)
	CD3DX12_ROOT_PARAMETER1 rootParameters[1];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	if (!m_Device.CreateRootSignature(rootParameters, _countof(rootParameters), m_pRootSignature.GetAddressOf()))
		return false;

	// Create PSOs
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
	psDesc.RootSignature = m_pRootSignature.Get();

	if (!m_Device.CreatePipelineState(psDesc, m_pBasicPipelineState.GetAddressOf()))
		return false;

	return true;
}
void RendererD3D12::Update(void)
{
	PRAGMA_TODO("Refactor into Object class")
	// Update the model matrix. 
	float angle = static_cast<float>(90.0);
	const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
	m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));

	PRAGMA_TODO("Refactor into Camera Class")
	// Update the view matrix.
	const XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
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
		m_pGFXCommandList->SetPipelineState(m_pBasicPipelineState.Get());
		m_pGFXCommandList->SetGraphicsRootSignature(m_pRootSignature.Get());
		m_pSwapChain->SetOMRenderTargets(m_pGFXCommandList);

		// Per Object Draws
		{
			m_pGFXCommandList->SetIAPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pGFXCommandList->SetIAVertexBuffers(0, 1, &m_pVertexBuffer->GetView());
			m_pGFXCommandList->SetIAIndexBuffer(&m_pIndexBuffer->GetView());

PRAGMA_TODO("Constant Buffer objects")
			// Constant Buffer Information    
			XMMATRIX mvpMatrix = XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
			mvpMatrix = XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
			m_pGFXCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

			m_pGFXCommandList->DrawIndexedInstanced(_countof(g_Indicies), 1, 0, 0, 0);
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