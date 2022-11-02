#include <iostream>

#include <chrono>
#include <Windows.h>

#include <filesystem>
#include <shlobj.h>

#include "Defines.h"

#include "Window/GameWindow.h"
#include "Loaders/CLParser.h"
#include "Logging/Logger.h"
#include "Factory/RendererFactory.h"

//#include "InputManager.h"

PRAGMA_TODO( "Data Driven Pipelines" )
PRAGMA_TODO( "\tSamplerState Table" )

PRAGMA_TODO( "Scene Configuration File" )

PRAGMA_TODO( "Fullscreen Toggle" )
PRAGMA_TODO( "Variable Window Size" )
PRAGMA_TODO( "\tResize Handling" )

PRAGMA_TODO( "MT Command Buffers" )
PRAGMA_TODO( "\t - Submission / Execution needs proper synchronisation" )

static Artemis::Core::GameWindow* g_pWindow   = nullptr;
static Artemis::Renderer::Interfaces::IRenderer*  g_pRenderer = nullptr;

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
	Artemis::Utilities::Logger::SetSeverity(Artemis::Utilities::SEVERITY_INFO );
	Artemis::Utilities::Logger::SetCategory(Artemis::Utilities::CATEGORY_UTILITIES );

	Artemis::Utilities::CLParser::Instance()->Initialise( _pCmds );

	unsigned int uiLogCategory = Artemis::Utilities::CATEGORY_NONE;
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogCat_App" ) )
	{
		uiLogCategory |= Artemis::Utilities::CATEGORY_APP;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogCat_Renderer" ) )
	{
		uiLogCategory |= Artemis::Utilities::CATEGORY_RENDERER;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogCat_Utilities" ) )
	{
		uiLogCategory |= Artemis::Utilities::CATEGORY_UTILITIES;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogCat_Shaders" ) )
	{
		uiLogCategory |= Artemis::Utilities::CATEGORY_SHADERS;
	}

	unsigned int uiLogSeverity = Artemis::Utilities::SEVERITY_NONE;
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogSev_Info" ) )
	{
		uiLogSeverity |= Artemis::Utilities::SEVERITY_INFO;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogSev_Warn" ) )
	{
		uiLogSeverity |= Artemis::Utilities::SEVERITY_WARN;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogSev_Err" ) )
	{
		uiLogSeverity |= Artemis::Utilities::SEVERITY_ERR;
	}
	if (Artemis::Utilities::CLParser::Instance()->HasArgument( "LogSev_Fatal" ) )
	{
		uiLogSeverity |= Artemis::Utilities::SEVERITY_FATAL;
	}

	Artemis::Utilities::Logger::SetSeverity( uiLogSeverity );
	Artemis::Utilities::Logger::SetCategory( uiLogCategory );

#if defined(_DEBUG)
	if ( GetModuleHandle( L"WinPixGpuCapturer.dll" ) == nullptr )
	{
		LoadLibrary( GetLatestWinPixGpuCapturerPathCpp17().c_str() );
	}
#endif

	g_pWindow = new Artemis::Core::GameWindow( _hInstance, L"MainWindow", L"DX12 Project" );
	g_pRenderer = Artemis::Renderer::Techniques::RendererFactory::Construct(Artemis::Renderer::Techniques::RendererTechnique::ForwardRenderer);

	if ( !g_pRenderer->Initialise( g_pWindow ) )
	{
		return false;
	}

	Artemis::Core::GameWindow::SetMessageHandlerInstance( g_pWindow );

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
