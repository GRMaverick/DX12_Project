module;

#include <ImGUI\imgui_impl_win32.h>

module Artemis.Core:Window;

import "Defines.h";
import "ArtemisWindow.h";

import :CommandLine;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ArtemisCore::Window
{
	static ArtemisWindow* g_pWindow = nullptr;

	LRESULT CALLBACK WndProc(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(_hwnd, _umsg, _wparam, _lparam))
			return true;

		if (g_pWindow)
			return g_pWindow->MessageHandler(_hwnd, _umsg, _wparam, _lparam);
		else
			return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
	}

	LRESULT ArtemisWindow::MessageHandler(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
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

	ArtemisWindow::ArtemisWindow(HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
	{
		m_hWindow = nullptr;
		m_hInstance = _hInstance;
		m_ResizeDelegate = OnResizeDelegate();

		RegisterWindowClass(_pClassName);
		OpenWindow(_pClassName, _pWindowName, _bFullscreen);
	}

	ArtemisWindow::~ArtemisWindow(void)
	{
		CloseWindow(m_hWindow);
	}

	void ArtemisWindow::RegisterWindowClass(const wchar_t* _pClassName)
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

	void ArtemisWindow::OpenWindow(const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
	{
		LONG screenWidth = CommandLine::HasArgument("Width") ? atol(CommandLine::GetArgument("Width")) : 1920;
		LONG screenHeight = CommandLine::HasArgument("Height") ? atol(CommandLine::GetArgument("Height")) : 1080;

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

	void ArtemisWindow::Resize(UINT32 _width, UINT32 _height)
	{
		if (m_ResizeDelegate != nullptr)
			m_ResizeDelegate(_width, _height);
	}

	void ArtemisWindow::AddOnResizeDelegate(OnResizeDelegate _pCallback)
	{
		m_ResizeDelegate = _pCallback;
	}

	void ArtemisWindow::SetFullscreen(bool _bFullscreen)
	{

	}

	void ArtemisWindow::SetMessageHandlerInstance(ArtemisWindow* _pWindow)
	{
		g_pWindow = _pWindow;
	}

	HWND ArtemisWindow::GetWindowHandle(void) const
	{
		return m_hWindow;
	}

	HINSTANCE ArtemisWindow::GetInstanceHandle(void) const
	{
		return m_hInstance;
	}

	ArtemisWindow::Dimensions ArtemisWindow::GetDimensions(void) const
	{
		return m_Dimensions;
	}
} // namespace ArtemisCore::Window