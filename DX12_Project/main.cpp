#include <iostream>

#include <chrono>
#include <Windows.h>

#include <filesystem>
#include <shlobj.h>

#include "Defines.h"

#include "Window/GameWindow.h"
#include "Loaders/CLParser.h"
#include "Logging/Logger.h"
#include "Interfaces/IRenderer.h"

//#include "Artemis.Rendering\_Platforms\D3D12\RendererD3D12.h"

//#include "InputManager.h"

PRAGMA_TODO( "Data Driven Pipelines" )
PRAGMA_TODO( "\tSamplerState Table" )

PRAGMA_TODO( "Scene Configuration File" )

PRAGMA_TODO( "Fullscreen Toggle" )
PRAGMA_TODO( "Variable Window Size" )
PRAGMA_TODO( "\tResize Handling" )

PRAGMA_TODO( "MT Command Buffers" )
PRAGMA_TODO( "\t - Submission / Execution needs proper synchronisation" )

using namespace Artemis::Core;
using namespace Artemis::Utilities;
using namespace Artemis::Renderer::Interfaces;

static GameWindow* g_pWindow   = nullptr;
static IRenderer*  g_pRenderer = nullptr;

static std::wstring GetLatestWinPixGpuCapturerPathCpp17()
{
	LPWSTR programFilesPath = nullptr;
	SHGetKnownFolderPath( FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, nullptr, &programFilesPath );

	std::filesystem::path pixInstallationPath = programFilesPath;
	pixInstallationPath /= "Microsoft PIX";

	std::wstring newestVersionFound;

	for ( auto const& directory_entry : std::filesystem::directory_iterator( pixInstallationPath ) )
	{
		if ( directory_entry.is_directory() )
		{
			if ( newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str() )
			{
				newestVersionFound = directory_entry.path().filename().c_str();
			}
		}
	}

	if ( newestVersionFound.empty() )
	{
		// TODO: Error, no PIX installation found
	}

	return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}

bool GameLoop()
{
	static uint64_t                           frameCounter   = 0;
	static double                             elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto                               t0 = clock.now();

	frameCounter++;
	const auto   t1        = clock.now();
	const double deltaTime = (t1 - t0).count() * 1e-9;
	t0                     = t1;

	elapsedSeconds += deltaTime;
	if ( elapsedSeconds > 1.0 )
	{
		char         buffer[500];
		const double fps = frameCounter / elapsedSeconds;
		sprintf_s( buffer, 500, "DX12 Project [FPS: %f]\n", fps );
		SetWindowTextA( g_pWindow->GetWindowHandle(), buffer );

		frameCounter   = 0;
		elapsedSeconds = 0.0;
	}

	//InputManager::Instance()->Update();

	g_pRenderer->Update( deltaTime );
	g_pRenderer->Render();

	return true;
}

int __stdcall WinMain( const HINSTANCE _hInstance, const HINSTANCE _hPreviousInstance, const LPSTR _pCmds, INT _nCmdShow )
{
	Logger::SetSeverity( SEVERITY_INFO );
	Logger::SetCategory( CATEGORY_UTILITIES );

	CLParser::Instance()->Initialise( _pCmds );

	unsigned int uiLogCategory = CATEGORY_NONE;
	if ( CLParser::Instance()->HasArgument( "LogCat_App" ) )
	{
		uiLogCategory |= CATEGORY_APP;
	}
	if ( CLParser::Instance()->HasArgument( "LogCat_Renderer" ) )
	{
		uiLogCategory |= CATEGORY_RENDERER;
	}
	if ( CLParser::Instance()->HasArgument( "LogCat_Utilities" ) )
	{
		uiLogCategory |= CATEGORY_UTILITIES;
	}
	if ( CLParser::Instance()->HasArgument( "LogCat_Shaders" ) )
	{
		uiLogCategory |= CATEGORY_SHADERS;
	}

	unsigned int uiLogSeverity = SEVERITY_NONE;
	if ( CLParser::Instance()->HasArgument( "LogSev_Info" ) )
	{
		uiLogSeverity |= SEVERITY_INFO;
	}
	if ( CLParser::Instance()->HasArgument( "LogSev_Warn" ) )
	{
		uiLogSeverity |= SEVERITY_WARN;
	}
	if ( CLParser::Instance()->HasArgument( "LogSev_Err" ) )
	{
		uiLogSeverity |= SEVERITY_ERR;
	}
	if ( CLParser::Instance()->HasArgument( "LogSev_Fatal" ) )
	{
		uiLogSeverity |= SEVERITY_FATAL;
	}

	Logger::SetSeverity( uiLogSeverity );
	Logger::SetCategory( uiLogCategory );

#if defined(_DEBUG)
	if ( GetModuleHandle( L"WinPixGpuCapturer.dll" ) == nullptr )
	{
		LoadLibrary( GetLatestWinPixGpuCapturerPathCpp17().c_str() );
	}
#endif

	g_pWindow = new GameWindow( _hInstance, L"MainWindow", L"DX12 Project" );
	//g_pRenderer = new SysRenderer::Renderer();

	if ( !g_pRenderer->Initialise( g_pWindow ) )
	{
		return false;
	}

	GameWindow::SetMessageHandlerInstance( g_pWindow );

	bool bClosing = false;

	MSG msg;
	ZeroMemory( &msg, sizeof(MSG) );

	while ( !bClosing )
	{
		if ( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if ( msg.message == WM_QUIT )
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
