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

static WORD g_Indicies[36] =
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

	if (!LoadShaders())
		return false;

	return true;
}

bool RendererD3D12::LoadCube(void)
{	
	m_pCopyCommandList->Reset();

	if (m_Device.CreateVertexBufferResource(m_pCopyCommandList, _countof(g_Vertices), sizeof(VertexPosColor), D3D12_RESOURCE_FLAG_NONE, (void*)g_Vertices, &m_pVertexBuffer))
		return false;

	if (m_Device.CreateIndexBufferResource(m_pCopyCommandList, _countof(g_Indicies), sizeof(WORD), D3D12_RESOURCE_FLAG_NONE, (void*)g_Indicies, &m_pIndexBuffer))
		return false;

	m_pCopyCommandQueue->ExecuteCommandLists(m_pCopyCommandList, 1);

	return true;
}

bool RendererD3D12::LoadShaders(void)
{
	m_ShaderCache = ShaderCache(SHADER_CACHE_LOCATION);

	D3D12_SHADER_BYTECODE basicVS = m_ShaderCache.GetShader("BasicVS.vs");
	D3D12_SHADER_BYTECODE basicPS = m_ShaderCache.GetShader("BasicPS.ps");

	return true;
}

bool RendererD3D12::Render(void)
{
	m_pGFXCommandList->Reset();
	
	m_pSwapChain->PrepareForRendering(m_pGFXCommandList);
	
	m_pSwapChain->PrepareForPresentation(m_pGFXCommandList);

	m_pGFXCommandQueue->ExecuteCommandLists(m_pGFXCommandList, 1);
	
	m_pSwapChain->Present();
		
	m_pGFXCommandQueue->Signal();
	
	m_pSwapChain->Swap();
	
	m_pGFXCommandQueue->Wait();

	return true;
}