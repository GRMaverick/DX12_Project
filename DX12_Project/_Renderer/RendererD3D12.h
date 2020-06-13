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
#include "D3D12\Resources\UploadBuffer.h"

#include "Camera.h"

#include <DirectXMath.h>

class RenderEntity;

struct ObjectCB
{
	DirectX::XMMATRIX MVP;
};

class RendererD3D12 : public IRenderer
{
public:
	RendererD3D12(void);
	~RendererD3D12(void);

	virtual bool Initialise(CoreWindow* _pWindow) override final;
	virtual void Update(double _deltaTime) override final;
	virtual bool Render(void) override final;

private:
	UINT											m_ModelCount;
	UINT											m_ObjectCBCount;
	bool											m_bNewModelsLoaded;

	SwapChain*										m_pSwapChain;

	Camera											m_Camera;
	RenderEntity**									m_pRenderEntity;

	ShaderCache										m_ShaderCache;

	DescriptorHeap*									m_pImGuiSRVHeap;
	DescriptorHeap*									m_pDescHeapSampler;

	UploadBuffer<ObjectCB>*							m_ObjectCBs;

	// Refactor Required
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pBasicRS = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pAlbedoRS = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pBasicPSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pAlbedoPSO = nullptr;

	bool LoadContent();
	bool CreatePipelineState();

	void UpdatePassConstants();

	void MainRenderPass(CommandList* _pGfxCmdList);
};

#endif // __RendererD3D12_h__
