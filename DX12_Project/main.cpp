#include <iostream>

#include <Windows.h>
#include <chrono>

#include "Defines.h"

#include "SysCore\_Window\GameWindow.h"
#include "SysUtilities\_Loaders\CLParser.h"

#include "SysRenderer\_Platforms\D3D12\RendererD3D12.h"

#include "InputManager.h"

PRAGMA_TODO("Data Driven Pipelines")
PRAGMA_TODO("\tSamplerState Table")

PRAGMA_TODO("Scene Configuration File")

PRAGMA_TODO("Fullscreen Toggle")
PRAGMA_TODO("Variable Window Size")
PRAGMA_TODO("\tResize Handling")

PRAGMA_TODO("MT Command Buffers")
PRAGMA_TODO("\t - Submission / Execution needs proper synchronisation")

using namespace SysCore;
using namespace SysUtilities;

static GameWindow*	g_pWindow = nullptr;
static IRenderer*	g_pRenderer = nullptr;

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
	Logger::SetSeverity(SEVERITY_INFO);
	Logger::SetCategory(CATEGORY_UTILITIES);

	CLParser::Instance()->Initialise(pCmds);

	unsigned int uiLogCategory = CATEGORY_NONE;
	if (CLParser::Instance()->HasArgument("LogCat_App"))
	{
		uiLogCategory |= CATEGORY_APP;
	}
	if (CLParser::Instance()->HasArgument("LogCat_Renderer"))
	{
		uiLogCategory |= CATEGORY_RENDERER;
	}
	if (CLParser::Instance()->HasArgument("LogCat_Utilities"))
	{
		uiLogCategory |= CATEGORY_UTILITIES;
	}

	unsigned int uiLogSeverity = SEVERITY_NONE;
	if (CLParser::Instance()->HasArgument("LogSev_Info"))
	{
		uiLogSeverity |= SEVERITY_INFO;
	}
	if (CLParser::Instance()->HasArgument("LogSev_Warn"))
	{
		uiLogSeverity |= SEVERITY_WARN;
	}
	if (CLParser::Instance()->HasArgument("LogSev_Err"))
	{
		uiLogSeverity |= SEVERITY_ERR;
	}
	if (CLParser::Instance()->HasArgument("LogSev_Fatal"))
	{
		uiLogSeverity |= SEVERITY_FATAL;
	}

	Logger::SetSeverity(uiLogSeverity);
	Logger::SetCategory(uiLogCategory);

	g_pWindow = new GameWindow(hInstance, L"MainWindow", L"DX12 Project");
	g_pRenderer = new Renderer();

	if (!g_pRenderer->Initialise(g_pWindow))
	{
		return false;
	}

	GameWindow::SetMessageHandlerInstance(g_pWindow);

	bool bClosing = false;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

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
	return 0;
}