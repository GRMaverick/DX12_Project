#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <d3d12.h>

#include "Artemis.Rendering/_Interfaces/IRenderer.h"

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

		virtual bool Initialise( Artemis::Core::GameWindow* _pWindow ) override final;
		virtual void Update( double _deltaTime ) override final;
		virtual bool Render( void ) override final;

	private:
		bool m_bNewModelsLoaded;

		D3D12::SwapChain* m_pSwapChain;

		//Spotlight* m_spotlight;

		D3D12::DescriptorHeap* m_pImGuiSrvHeap;

		D3D12::ConstantBufferResource* m_pLightsCb;
		D3D12::ConstantBufferResource* m_pMainPassCb;
		//D3D12::ConstantBufferResource* m_pSpotlightCb;

		std::vector<Light*>               m_vpLights;
		std::vector<Scene::Camera*>       m_vpCameras;
		std::vector<Scene::RenderEntity*> m_vpRenderEntities;

		bool LoadContent();
		bool LoadScene( const std::string& _sceneFile );

		void UpdatePassConstants() const;

		void MainRenderPass( D3D12::CommandList* _pGfxCmdList ) const;
		void ImGuiPass( D3D12::CommandList* _pGfxCmdList ) const;
	};
}

#endif // __RendererD3D12_h__
