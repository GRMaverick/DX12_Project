#ifndef __Renderer_h__
#define __Renderer_h__

class CoreWindow;

class IRenderer
{
public:
	virtual bool Initialise(CoreWindow* _pWindow) = 0;
	virtual void Update() = 0;
	virtual bool Render() = 0;
};

#endif // __Renderer_h__