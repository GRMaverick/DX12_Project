#ifndef __Renderer_h__
#define __Renderer_h__

namespace SysCore
{
	class GameWindow;
}

namespace SysRenderer
{
	namespace Interfaces
	{
		class IRenderer
		{
		public:
			virtual bool Initialise(SysCore::GameWindow* _pWindow) = 0;
			virtual void Update(double _deltaTime) = 0;
			virtual bool Render() = 0;
		};
	}
}

#endif // __Renderer_h__