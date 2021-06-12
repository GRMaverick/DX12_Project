#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <wrl.h>

#include <d3d12.h>

#include "SysRenderer/_Interfaces/IRenderer.h"

class Camera;
class SwapChain;
class CommandList;
class RenderEntity;
class DescriptorHeap;
class ConstantBufferResource;

struct Light;

class Renderer : public IRenderer
{
public:
	Renderer(void);
	~Renderer(void);

	virtual bool Initialise(GameWindow* _pWindow) override final;
	virtual void Update(double _deltaTime) override final;
	virtual bool Render(void) override final;

private:
	UINT											m_ModelCount;
	bool											m_bNewModelsLoaded;

	SwapChain*										m_pSwapChain;

	Light*											m_Light;
	Camera*											m_Camera;
	RenderEntity**									m_pRenderEntity;

	DescriptorHeap*									m_pImGuiSRVHeap;

	ConstantBufferResource*							m_pLightsCB;
	ConstantBufferResource*							m_pMainPassCB;
	bool LoadContent();

	void UpdatePassConstants();

	void MainRenderPass(CommandList* _pGfxCmdList);
	void ImGuiPass(CommandList* _pGfxCmdList);
};

#endif // __RendererD3D12_h__
