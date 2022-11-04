#ifndef __ImGUIEngine_h__
#define __ImGUIEngine_h__

#include <ImGUI\imgui.h>

#include "Defines.h"

//PRAGMA_TODO("Remove Windows Platform From Here")
#include <Windows.h>

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Interfaces
{
	class ICommandList;
	class IGraphicsDevice;
	class IDescriptorHeap;
}

class ImGUIEngine
{
public:
	//
	// Call from Device
	//
	static void Initialise( Artemis::Core::GameWindow* _pWindow, const Artemis::Renderer::Interfaces::IGraphicsDevice* _pDevice, const Artemis::Renderer::Interfaces::IDescriptorHeap* _pSRVHeap );

	//
	// Call from Renderer
	static void Begin();
	static void Update();
	static void DrawSlider3f( const char* _pLabel, float* _pX, float* _pY, float* _pZ );
	static void End();

	//
	// Call from CommandList
	//
	static void Draw( const Artemis::Renderer::Interfaces::ICommandList* _pCommandList );

private:
	static ImGuiIO* m_IO;
};

#endif __ImGUIEngine_h__
