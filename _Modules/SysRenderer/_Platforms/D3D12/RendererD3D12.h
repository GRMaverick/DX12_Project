#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <d3d12.h>

#include "SysRenderer/_Interfaces/IRenderer.h"

struct Light;
struct Spotlight;

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

	class Renderer final : public Interfaces::IRenderer
	{
	public:
		Renderer( void );
		~Renderer( void ) override;

		virtual bool Initialise( SysCore::GameWindow* _pWindow ) override final;
		virtual void Update( double _deltaTime ) override final;
		virtual bool Render( void ) override final;

	private:
		UINT m_uiModelCount;
		bool m_bNewModelsLoaded;

		D3D12::SwapChain* m_pSwapChain;

		Light*     m_light;
		Spotlight* m_spotlight;

		Scene::Camera*        m_pCamera;
		Scene::RenderEntity** m_pRenderEntity;

		D3D12::DescriptorHeap* m_pImGuiSrvHeap;

		D3D12::ConstantBufferResource* m_pLightsCb;
		D3D12::ConstantBufferResource* m_pMainPassCb;
		D3D12::ConstantBufferResource* m_pSpotlightCb;

		bool LoadContent();

		void UpdatePassConstants() const;

		void MainRenderPass( D3D12::CommandList* _pGfxCmdList ) const;
		void ImGuiPass( D3D12::CommandList* _pGfxCmdList ) const;
	};
}

#endif // __RendererD3D12_h__
