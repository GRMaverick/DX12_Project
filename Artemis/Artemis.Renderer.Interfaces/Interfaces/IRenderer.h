#ifndef __Renderer_h__
#define __Renderer_h__

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Interfaces
{
	class IRenderer
	{
	public:
		virtual      ~IRenderer() = default;
		virtual bool Initialise( Core::GameWindow* _pWindow ) = 0;
		virtual void Update( double _deltaTime ) = 0;
		virtual bool Render() = 0;
	};
}

#endif // __Renderer_h__
