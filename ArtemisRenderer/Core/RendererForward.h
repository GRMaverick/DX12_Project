#ifndef __RendererForward_h__
#define __RendererForward_h__

import <Windows.h>;

import "IRenderer.h";

import "Scene/Camera.h";
import "Scene/RenderEntity.h";

import "Device/SwapChain.h";

import "Resources/CBStructures.h";
import "Resources/ConstantBuffer.h";
import "Resources/DescriptorHeap.h";

namespace ArtemisRenderer::Core
{
	class RendererForward final : public IRenderer
	{
	public:
		RendererForward(void);
		~RendererForward(void);

		virtual bool Initialise(ArtemisCore::Window::ArtemisWindow* _pWindow) override final;
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

		Resources::ConstantBuffer*	m_pLightsCB;
		Resources::ConstantBuffer*	m_pMainPassCB;
		Resources::ConstantBuffer*	m_pSpotlightCB;

		bool LoadContent();

		void UpdatePassConstants();

		void MainRenderPass(Device::CommandList* _pGfxCmdList);
		void ImGuiPass(Device::CommandList* _pGfxCmdList);
	};
}

#endif // __RendererForward_h__
