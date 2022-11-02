#include "GameWindow.h"

#include "Loaders/CLParser.h"

//#include "InputManager.h"

#include <algorithm>
#include <cassert>
#include <utility>

#include <ImGUI/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

using namespace Artemis::Utilities;

namespace Artemis::Core
{
	static GameWindow* g_pWindow = nullptr;

	LRESULT CALLBACK WndProc( const HWND _hwnd, const UINT _umsg, const WPARAM _wparam, const LPARAM _lparam )
	{
		if ( ImGui_ImplWin32_WndProcHandler( _hwnd, _umsg, _wparam, _lparam ) )
			return true;

		if ( g_pWindow )
			return g_pWindow->MessageHandler( _hwnd, _umsg, _wparam, _lparam );
		else
			return DefWindowProc( _hwnd, _umsg, _wparam, _lparam );
	}

	LRESULT GameWindow::MessageHandler( const HWND _hwnd, const UINT _umsg, const WPARAM _wparam, const LPARAM _lparam ) const
	{
		switch ( _umsg )
		{
			case WM_SIZE:
			{
				RECT rect = {};
				GetWindowRect( _hwnd, &rect );
				Resize( rect.right - rect.left, rect.bottom - rect.top );
				return 0;
			}
			default:
				return DefWindowProc( _hwnd, _umsg, _wparam, _lparam );
		}
	}

	GameWindow::GameWindow( void ):
		m_HWindow( nullptr ),
		m_HInstance( nullptr )
	{
	}

	GameWindow::GameWindow( const GameWindow& _rhs ) :
		m_HWindow( nullptr ),
		m_HInstance( nullptr )
	{
	}

	GameWindow::GameWindow( HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen )
	{
		m_HWindow         = nullptr;
		m_HInstance       = _hInstance;
		m_dResizeDelegate = OnResizeDelegate();

		RegisterWindowClass( _pClassName );
		OpenWindow( _pClassName, _pWindowName, _bFullscreen );
	}

	GameWindow::~GameWindow( void )
	{
		CloseWindow( m_HWindow );
	}

	void GameWindow::RegisterWindowClass( const wchar_t* _pClassName ) const
	{
		WNDCLASSEXW wClass = {};
		ZeroMemory( &wClass, sizeof(WNDCLASSEXW) );
		wClass.cbSize        = sizeof( WNDCLASSEXW );
		wClass.style         = CS_HREDRAW | CS_VREDRAW;
		wClass.lpfnWndProc   = WndProc;
		wClass.cbClsExtra    = 0;
		wClass.cbWndExtra    = 0;
		wClass.hInstance     = m_HInstance;
		wClass.hIcon         = ::LoadIconW( m_HInstance, nullptr );
		wClass.hCursor       = ::LoadCursor( nullptr, IDC_ARROW );
		wClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		wClass.lpszMenuName  = nullptr;
		wClass.lpszClassName = _pClassName;
		wClass.hIconSm       = ::LoadIcon( m_HInstance, nullptr );

		static ATOM wndReg = ::RegisterClassExW( &wClass );
		assert( wndReg > 0 );
	}

	void GameWindow::OpenWindow( const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen )
	{
		LONG screenWidth = 1920;
		if ( CLParser::Instance()->HasArgument( "Width" ) )
		{
			screenWidth = atol( CLParser::Instance()->GetArgument( "Width" ) );
		}

		LONG screenHeight = 1080;
		if ( CLParser::Instance()->HasArgument( "Height" ) )
		{
			screenHeight = atol( CLParser::Instance()->GetArgument( "Height" ) );
		}

		RECT windowRect = {0, 0, screenWidth, screenHeight};
		::AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

		const INT windowWidth  = windowRect.right - windowRect.left;
		const INT windowHeight = windowRect.bottom - windowRect.top;
		const INT windowX      = std::max<INT>( 0, screenWidth - windowWidth );
		const INT windowY      = std::max<INT>( 0, screenHeight - windowHeight );

		m_Dimensions.WindowWidth  = windowWidth;
		m_Dimensions.WindowHeight = windowHeight;
		m_Dimensions.ScreenWidth  = screenWidth;
		m_Dimensions.ScreenHeight = screenHeight;
		m_Dimensions.IsFullscreen = _bFullscreen;

		m_HWindow = ::CreateWindowExW( 0, _pClassName, _pWindowName, WS_OVERLAPPEDWINDOW, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, m_HInstance, nullptr );
		assert( m_HWindow && "Failed to create window" );

		ShowWindow( m_HWindow, SW_SHOW );
		SetForegroundWindow( m_HWindow );
		SetFocus( m_HWindow );
		ShowCursor( true );
	}

	void GameWindow::Resize( const UINT32 _width, const UINT32 _height ) const
	{
		if ( m_dResizeDelegate != nullptr )
			m_dResizeDelegate( _width, _height );
	}

	void GameWindow::AddOnResizeDelegate( OnResizeDelegate _pCallback )
	{
		m_dResizeDelegate = std::move( _pCallback );
	}

	void GameWindow::SetFullscreen( bool _bFullscreen ) const
	{
	}

	void GameWindow::SetMessageHandlerInstance( GameWindow* _pWindow )
	{
		g_pWindow = _pWindow;
	}

	HWND GameWindow::GetWindowHandle( void ) const
	{
		return m_HWindow;
	}

	HINSTANCE GameWindow::GetInstanceHandle( void ) const
	{
		return m_HInstance;
	}

	GameWindow::Dimensions GameWindow::GetDimensions( void ) const
	{
		return m_Dimensions;
	}
}
