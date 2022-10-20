#ifndef __IRenderer_h__
#define __IRenderer_h__

namespace ArtemisCore::Window
{
	class ArtemisWindow;
}

namespace ArtemisRenderer::Core
{
	class IRenderer
	{
	public:
		virtual bool Initialise(ArtemisCore::Window::ArtemisWindow* _pWindow) = 0;
		virtual void Update(double _deltaTime) = 0;
		virtual bool Render() = 0;
	};
}

#endif // __IRenderer_h__