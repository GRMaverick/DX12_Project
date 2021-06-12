#ifndef __ImGUIEngine_h__
#define __ImGUIEngine_h__

#include <ImGUI\imgui.h>

#include "Defines.h"

PRAGMA_TODO("Remove Windows Platform From Here")
#include <Windows.h>

class CommandList;
class DescriptorHeap;

class ImGUIEngine
{
public:
	//
	// Call from Device
	//
	static void Initialise(HWND _hWindow, DescriptorHeap* _pSRVHeap);

	//
	// Call from Renderer
	static void Begin();
	static void Update();
	static void DrawSlider3f(const char* _pLabel, float* _pX, float* _pY, float* _pZ);
	static void End();

	//
	// Call from CommandList
	//
	static void Draw(CommandList* _pCommandList);

private:
	static ImGuiIO* m_IO;
};

#endif __ImGUIEngine_h__