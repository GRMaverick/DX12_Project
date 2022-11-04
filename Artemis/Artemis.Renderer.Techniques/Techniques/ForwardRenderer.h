#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <d3d12.h>
#include <string>
#include <vector>

#include "Interfaces/IRenderer.h"

namespace Artemis
{
	namespace Renderer
	{
		namespace Assets
		{
			class RenderEntity;
			class Camera;
		}
	}
}

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Interfaces
{
	class IGpuResource;
	class IDescriptorHeap;
	class ISwapChain;
	class ICommandList;
	class ICommandQueue;
	class IGraphicsDevice;
}

struct Light;
struct Spotlight;

namespace Artemis::Renderer::Techniques
{
	namespace Scene
	{
		class Camera;
		class RenderEntity;
	}

	class ForwardRenderer final : public Interfaces::IRenderer
	{
	public:
		ForwardRenderer( void );
		~ForwardRenderer( void ) override;

		virtual bool Initialise( Artemis::Core::GameWindow* _pWindow ) override final;
		virtual void Update( double _deltaTime ) override final;
		virtual bool Render( void ) override final;

	private:
		bool m_bNewModelsLoaded;

		Artemis::Renderer::Interfaces::IGraphicsDevice* m_pDevice      = nullptr;
		Artemis::Renderer::Interfaces::ICommandQueue*   m_pGfxCmdQueue = nullptr;
		Artemis::Renderer::Interfaces::ICommandQueue*   m_pCpyCmdQueue = nullptr;

		Artemis::Renderer::Interfaces::ISwapChain* m_pSwapChain;

		//Spotlight* m_spotlight;

		Artemis::Renderer::Interfaces::IDescriptorHeap* m_pImGuiSrvHeap;

		Interfaces::IGpuResource* m_pLightsCb;
		Interfaces::IGpuResource* m_pMainPassCb;
		Interfaces::IGpuResource* m_pSpotlightCb;

		std::vector<Light*>                                   m_vpLights;
		std::vector<Artemis::Renderer::Assets::Camera*>       m_vpCameras;
		std::vector<Artemis::Renderer::Assets::RenderEntity*> m_vpRenderEntities;

		bool LoadContent();
		bool LoadScene( const std::string& _sceneFile );

		void UpdatePassConstants() const;

		void MainRenderPass( const Interfaces::ICommandList* _pGfxCmdList ) const;
		void ImGuiPass( const Interfaces::ICommandList* _pGfxCmdList ) const;
	};
}

#endif // __RendererD3D12_h__
