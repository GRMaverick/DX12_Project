#include <Windows.h>
#include <string.h>
#include <chrono>

#include "InputManager.h"

import Artemis.Core;
using namespace ArtemisCore;

import Artemis.Renderer;
using namespace ArtemisRenderer::Core;

Window::ArtemisWindow* g_pWindow;
ArtemisRenderer::Core::IRenderer* g_pRenderer = nullptr;

bool GameLoop()
{
	static uint64_t frameCounter = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = clock.now();

	frameCounter++;
	auto t1 = clock.now();
	double deltaTime = (t1 - t0).count() * 1e-9;
	t0 = t1;

	elapsedSeconds += deltaTime;
	if (elapsedSeconds > 1.0)
	{
		char buffer[500];
		auto fps = frameCounter / elapsedSeconds;
		sprintf_s(buffer, 500, "DX12 Project [FPS: %f]\n", fps);
		SetWindowTextA(g_pWindow->GetWindowHandle(), buffer);

		frameCounter = 0;
		elapsedSeconds = 0.0;
	}

	InputManager::Instance()->Update();

	g_pRenderer->Update(deltaTime);
	g_pRenderer->Render();

	return true;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR pCmds, INT nCmdShow)
{
	char pBuffer[] = "Hello World!";
	Hashing::ArtemisHash hash = Hashing::SimpleHash(pBuffer, ARRAYSIZE(pBuffer));

	bool bRes = CommandLine::ParseCommandLine(pCmds);
	bool bRes2 = CommandLine::HasArgument("Width");
	bool bRes3 = CommandLine::HasArgument("Height");
	const char* pArg1 = CommandLine::GetArgument("Width");
	const char* pArg2 = CommandLine::GetArgument("Height");

	Logging::ArtemisLogger::SetCategory(Logging::ArtemisLogger::CATEGORY_APP);
	Logging::ArtemisLogger::SetSeverity(Logging::ArtemisLogger::SEVERITY_INFO);

	Logging::ArtemisLogger::Log(
		Logging::ArtemisLogger::SEVERITY_INFO,
		Logging::ArtemisLogger::CATEGORY_APP,
		"Finished!"
	);

	g_pWindow = new Window::ArtemisWindow(hInstance, L"Artemis - Modules", L"Artemis - Modules", false);
	g_pRenderer = new ArtemisRenderer::Core::RendererForward();
	if (g_pRenderer->Initialise(g_pWindow))
	{
		return false;
	}

	bool bClosing = false;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (!bClosing)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			bClosing = true;
			continue;
		}
		else
		{
			GameLoop();
		}
	}

	return true;
}