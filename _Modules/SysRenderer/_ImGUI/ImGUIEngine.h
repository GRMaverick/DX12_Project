#ifndef __ImGUIEngine_h__
#define __ImGUIEngine_h__

#include <ImGUI\imgui.h>

#include "Defines.h"

PRAGMA_TODO( "Remove Windows Platform From Here" )
#include <Windows.h>

namespace SysRenderer
{
	namespace D3D12
	{
		class CommandList;
		class DescriptorHeap;
	}

	namespace ImGuiUtils
	{
		class ImGUIEngine
		{
		public:
			//
			// Call from Device
			//
			static void Initialise( HWND _hWindow, D3D12::DescriptorHeap* _pSRVHeap );

			//
			// Call from Renderer
			static void Begin();
			static void Update();
			static void DrawSlider3F( const char* _pLabel, float* _pX, float* _pY, float* _pZ );
			static void End();

			//
			// Call from CommandList
			//
			static void Draw( D3D12::CommandList* _pCommandList );

		private:
			static ImGuiIO* m_io;
		};
	}
}

#endif __ImGUIEngine_h__
