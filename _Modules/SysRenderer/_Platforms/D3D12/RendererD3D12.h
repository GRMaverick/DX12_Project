#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <wrl.h>

#include <d3d12.h>

#include "SysRenderer/_Interfaces/IRenderer.h"

struct Light;

namespace SysRenderer
{
	namespace Scene
	{
		class Camera;
		class RenderEntity;
	}

	namespace D3D12
	{
		class CommandList;
		class SwapChain;
		class DescriptorHeap;
		class ConstantBufferResource;
	}

	class Renderer : public Interfaces::IRenderer
	{
	public:
		Renderer(void);
		~Renderer(void);

		virtual bool Initialise(SysCore::GameWindow* _pWindow) override final;
		virtual void Update(double _deltaTime) override final;
		virtual bool Render(void) override final;

	private:
		UINT											m_ModelCount;
		bool											m_bNewModelsLoaded;

		D3D12::SwapChain* m_pSwapChain;

		Light* m_Light;
		Scene::Camera* m_Camera;
		Scene::RenderEntity** m_pRenderEntity;

		D3D12::DescriptorHeap* m_pImGuiSRVHeap;

		D3D12::ConstantBufferResource* m_pLightsCB;
		D3D12::ConstantBufferResource* m_pMainPassCB;
		bool LoadContent();

		void UpdatePassConstants();

		void MainRenderPass(D3D12::CommandList* _pGfxCmdList);
		void ImGuiPass(D3D12::CommandList* _pGfxCmdList);
	};
}

#endif // __RendererD3D12_h__
