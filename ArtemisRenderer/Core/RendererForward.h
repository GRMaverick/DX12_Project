#ifndef __RendererForward_h__
#define __RendererForward_h__

#include <d3d12.h>

#include "IRenderer.h"

namespace ArtemisRenderer::Device
{
	class CommandList;
	class SwapChain;
}

namespace ArtemisRenderer::Resources
{
	struct Light;
	struct Spotlight;

	class DescriptorHeap;
	class ConstantBuffer;
}	

namespace ArtemisRenderer::Scene
{
	class Camera;
	class RenderEntity;
}

namespace ArtemisRenderer::Core
{
	class Renderer : public IRenderer
	{
	public:
		Renderer(void);
		~Renderer(void);

		virtual bool Initialise(SysCore::GameWindow* _pWindow) override final;
		virtual void Update(double _deltaTime) override final;
		virtual bool Render(void) override final;

	private:
		UINT						m_ModelCount;
		bool						m_bNewModelsLoaded;

		Device::SwapChain*			m_pSwapChain;

		Resources::Light*			m_Light;
		Resources::Spotlight*		m_Spotlight;

		Scene::Camera*				m_Camera;
		Scene::RenderEntity**		m_pRenderEntity;

		Resources::DescriptorHeap*	m_pImGuiSRVHeap;

		Resources::ConstantBuffer* m_pLightsCB;
		Resources::ConstantBuffer* m_pMainPassCB;
		Resources::ConstantBuffer* m_pSpotlightCB;

		bool LoadContent();

		void UpdatePassConstants();

		void MainRenderPass(Device::CommandList* _pGfxCmdList);
		void ImGuiPass(Device::CommandList* _pGfxCmdList);
	};
}

#endif // __RendererForward_h__
