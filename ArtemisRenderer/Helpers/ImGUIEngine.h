#ifndef __ImGUIEngine_h__
#define __ImGUIEngine_h__

#include <ImGUI\imgui.h>

#include "Defines.h"

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

//PRAGMA_TODO("Remove Windows Platform From Here")
#include <Windows.h>

namespace ArtemisRenderer::Device
{
	class CommandList;
}

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap;
}

namespace ArtemisRenderer::Helpers
{
	class ImGUIEngine
	{
	public:
		//
		// Call from Device
		//
		static void Initialise(HWND _hWindow, Resources::DescriptorHeap* _pSRVHeap);

		//
		// Call from Renderer
		static void Begin();
		static void Update();
		static void DrawSlider3f(const char* _pLabel, float* _pX, float* _pY, float* _pZ);
		static void End();

		//
		// Call from CommandList
		//
		static void Draw(Device::CommandList* _pCommandList);

	private:
		static ImGuiIO* m_IO;
	};
}

#endif __ImGUIEngine_h__