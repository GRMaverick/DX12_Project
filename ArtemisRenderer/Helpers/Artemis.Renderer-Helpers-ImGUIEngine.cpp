module;

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#pragma comment(lib, "ImGUI.lib")

module Artemis.Renderer:Helpers;

import "Defines.h";

import "ImGUIEngine.h";

import "Device/RenderDevice.h";
import "Device/CommandList.h";

namespace ArtemisRenderer::Helpers
{
	ImGuiIO* ImGUIEngine::m_IO = nullptr;

	void ImGUIEngine::Initialise(HWND _hWindow, Resources::DescriptorHeap* _pSRVHeap)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		m_IO = &ImGui::GetIO();
		m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//m_IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

		Device::RenderDevice::Instance()->InitialiseImGUI(_hWindow, _pSRVHeap);

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

	void ImGUIEngine::Draw(Device::CommandList* _pCommandList)
	{
		_pCommandList->DrawImGUI();
	}
}