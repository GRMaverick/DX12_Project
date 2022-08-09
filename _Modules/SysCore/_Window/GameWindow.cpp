#include "Defines.h"
#include "GameWindow.h"

#include "SysUtilities\_Loaders\CLParser.h"

//#include "InputManager.h"

#include <assert.h>
#include <algorithm>

#include <ImGUI\imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SysCore
{
	static GameWindow* g_pWindow = nullptr;

	LRESULT CALLBACK WndProc(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(_hwnd, _umsg, _wparam, _lparam))
			return true;

		if (g_pWindow)
			return g_pWindow->MessageHandler(_hwnd, _umsg, _wparam, _lparam);
		else
			return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
	}

	LRESULT GameWindow::MessageHandler(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
	{
		switch (_umsg)
		{
		case WM_SIZE:
		{
			RECT rect = {};
			GetWindowRect(_hwnd, &rect);
			Resize(rect.right - rect.left, rect.bottom - rect.top);
			return 0;
		}
		default: return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
		}
	}

	GameWindow::GameWindow(HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
	{
		m_hWindow = nullptr;
		m_hInstance = _hInstance;
		m_ResizeDelegate = OnResizeDelegate();

		RegisterWindowClass(_pClassName);
		OpenWindow(_pClassName, _pWindowName, _bFullscreen);
	}

	GameWindow::~GameWindow(void)
	{
		CloseWindow(m_hWindow);
	}

	void GameWindow::RegisterWindowClass(const wchar_t* _pClassName)
	{
		WNDCLASSEXW wClass = {};
		ZeroMemory(&wClass, sizeof(WNDCLASSEXW));
		wClass.cbSize = sizeof(WNDCLASSEXW);
		wClass.style = CS_HREDRAW | CS_VREDRAW;
		wClass.lpfnWndProc = WndProc;
		wClass.cbClsExtra = 0;
		wClass.cbWndExtra = 0;
		wClass.hInstance = m_hInstance;
		wClass.hIcon = ::LoadIcon(m_hInstance, nullptr);
		wClass.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wClass.lpszMenuName = nullptr;
		wClass.lpszClassName = _pClassName;
		wClass.hIconSm = ::LoadIcon(m_hInstance, nullptr);

		static ATOM wndReg = ::RegisterClassExW(&wClass);
		assert(wndReg > 0);
	}

	void GameWindow::OpenWindow(const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
	{
		LONG screenWidth = 1920;
		if (CLParser::Instance()->HasArgument("Width"))
		{
			screenWidth = atol(CLParser::Instance()->GetArgument("Width"));
		}

		LONG screenHeight = 1080;
		if (CLParser::Instance()->HasArgument("Height"))
		{
			screenHeight = atol(CLParser::Instance()->GetArgument("Height"));
		}

		RECT windowRect = { 0, 0, screenWidth, screenHeight };
		::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		INT windowWidth = windowRect.right - windowRect.left;
		INT windowHeight = windowRect.bottom - windowRect.top;
		INT windowX = std::max<INT>(0, screenWidth - windowWidth);
		INT windowY = std::max<INT>(0, screenHeight - windowHeight);

		m_Dimensions.WindowWidth = windowWidth;
		m_Dimensions.WindowHeight = windowHeight;
		m_Dimensions.ScreenWidth = screenWidth;
		m_Dimensions.ScreenHeight = screenHeight;
		m_Dimensions.IsFullscreen = _bFullscreen;

		m_hWindow = ::CreateWindowExW(
			0,
			_pClassName,
			_pWindowName,
			WS_OVERLAPPEDWINDOW,
			windowX,
			windowY,
			windowWidth,
			windowHeight,
			nullptr,
			nullptr,
			m_hInstance,
			nullptr
		);
		assert(m_hWindow && "Failed to create window");

		ShowWindow(m_hWindow, SW_SHOW);
		SetForegroundWindow(m_hWindow);
		SetFocus(m_hWindow);
		ShowCursor(true);
	}

	void GameWindow::Resize(UINT32 _width, UINT32 _height)
	{
		if (m_ResizeDelegate != nullptr)
			m_ResizeDelegate(_width, _height);
	}

	void GameWindow::AddOnResizeDelegate(OnResizeDelegate _pCallback)
	{
		m_ResizeDelegate = _pCallback;
	}

	void GameWindow::SetFullscreen(bool _bFullscreen)
	{

	}

	void GameWindow::SetMessageHandlerInstance(GameWindow* _pWindow)
	{
		g_pWindow = _pWindow;
	}

	HWND GameWindow::GetWindowHandle(void) const
	{
		return m_hWindow;
	}

	HINSTANCE GameWindow::GetInstanceHandle(void) const
	{
		return m_hInstance;
	}

	GameWindow::Dimensions GameWindow::GetDimensions(void) const
	{
		return m_Dimensions;
	}
} // namespace SysCore