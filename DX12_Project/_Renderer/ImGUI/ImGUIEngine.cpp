#include "Defines.h"

#include "ImGUIEngine.h"
#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#pragma comment(lib, "ImGUI.lib")

ImGuiIO* ImGUIEngine::m_IO = nullptr;

void ImGUIEngine::Initialise(HWND _hWindow, DescriptorHeap* _pSRVHeap)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//m_IO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	DeviceD3D12::Instance()->InitialiseImGUI(_hWindow, _pSRVHeap);

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
	
	ImGui::Begin("Hello, ImGUI");
	ImGui::Text("Wassssup");
	ImGui::End();
}
void ImGUIEngine::End()
{
	ImGui::EndFrame();
}

void ImGUIEngine::Draw(CommandList* _pCommandList)
{
	_pCommandList->DrawImGUI();
}