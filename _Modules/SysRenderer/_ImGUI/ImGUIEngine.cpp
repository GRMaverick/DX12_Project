#include "Defines.h"

#include "ImGUIEngine.h"
#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#pragma comment(lib, "ImGUI.lib")

namespace SysRenderer
{
	namespace ImGUI
	{
		ImGuiIO* ImGUIEngine::m_IO = nullptr;

		void ImGUIEngine::Initialise(HWND _hWindow, D3D12::DescriptorHeap* _pSRVHeap)
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			m_IO = &ImGui::GetIO();
			m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			//m_IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

			ImGui::StyleColorsDark();

			D3D12::DeviceD3D12::Instance()->InitialiseImGUI(_hWindow, _pSRVHeap);

			//
			// Fonts (Optional)
			//
		}

		void ImGUIEngine::Begin()
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Update();
		}

		void ImGUIEngine::Update()
		{
			static bool bOpen = true;

		}

		void ImGUIEngine::End()
		{
			ImGui::EndFrame();
		}

		void ImGUIEngine::DrawSlider3f(const char* _pLabel, float* _pX, float* _pY, float* _pZ)
		{
			float v[3];
			if (ImGui::SliderFloat3(_pLabel, v, -1000.0f, 1000.0f))
			{
				*_pX = v[0];
				*_pY = v[1];
				*_pZ = v[2];
			}
		}

		void ImGUIEngine::Draw(D3D12::CommandList* _pCommandList)
		{
			_pCommandList->DrawImGUI();
		}
	}
}