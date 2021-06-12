#ifndef __Renderer_h__
#define __Renderer_h__

class GameWindow;

class IRenderer
{
public:
	virtual bool Initialise(GameWindow* _pWindow) = 0;
	virtual void Update(double _deltaTime) = 0;
	virtual bool Render() = 0;
};

#endif // __Renderer_h__