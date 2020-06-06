#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <wrl.h>

#include <d3d12.h>

#include "IRenderer.h"

#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"
#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\SwapChain.h"

#include "D3D12\Shaders\ShaderCache.h"

#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\VertexBufferResource.h"
#include "D3D12\Resources\Texture2DResource.h"

#include "Camera.h"

#include <DirectXMath.h>

class RenderEntity;

class RendererD3D12 : public IRenderer
{
public:
	RendererD3D12(void);
	~RendererD3D12(void);

	virtual bool Initialise(CoreWindow* _pWindow) override final;
	virtual void Update(float _deltaTime) override final;
	virtual bool Render(void) override final;

private:
	SwapChain*										m_pSwapChain;

	CommandList*									m_pGFXCommandList;
	CommandList*									m_pCopyCommandList;

	CommandQueue*									m_pCopyCommandQueue;
	CommandQueue*									m_pGFXCommandQueue;

	Camera											m_Camera;
	RenderEntity*									m_pRenderEntity;

	ShaderCache										m_ShaderCache;
	DescriptorHeap*									m_pDescHeapSampler;

	// Refactor Required
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pBasicRS = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pAlbedoRS = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pBasicPSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pAlbedoPSO = nullptr;

	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
	float m_AspectRatio;
	float m_FieldOfView;
	float m_CurrentRotation = 0.0f;

	bool LoadContent();
	bool CreatePipelineState();
};

#endif // __RendererD3D12_h__
