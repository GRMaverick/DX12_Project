#pragma once

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Interfaces
{
	class ICommandList;
	class ICommandQueue;
	class IGraphicsDevice;
	class IDescriptorHeap;

	class ISwapChain
	{
	public:
		virtual ~ISwapChain( void )
		{
		}

		virtual bool Initialise( const IGraphicsDevice* _pDevice, ICommandQueue* _pCommandQueue, unsigned int _backBuffers, IDescriptorHeap* _pDescHeapRTV, IDescriptorHeap* _pDescHeapDSV, Artemis::Core::GameWindow* _pWindow ) = 0;
		virtual bool Present( void ) const = 0;
		virtual void Swap( void ) = 0;

		virtual void PrepareForRendering( ICommandList* _pCmdList ) const = 0;
		virtual void PrepareForPresentation( ICommandList* _pCmdList ) const = 0;
		virtual void SetOmRenderTargets( ICommandList* _pCmdList ) const = 0;
	};
}
