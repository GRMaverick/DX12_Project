#include "Defines.h"
#include "CoreWindow.h"

#include <assert.h>
#include <algorithm>

PRAGMA_TODO("Fullscreen Toggle")
PRAGMA_TODO("Variable Window Size")

static CoreWindow* g_pWindow = nullptr;

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
{
	if (g_pWindow)
		return g_pWindow->MessageHandler(_hwnd, _umsg, _wparam, _lparam);
	else
		return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
}
LRESULT CoreWindow::MessageHandler(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam)
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
	case WM_KEYDOWN: return 0;
	case WM_KEYUP: return 0;
	default: return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);
	}
}

CoreWindow::CoreWindow(HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
{
	m_hWindow = nullptr;
	m_hInstance = _hInstance;
	m_ResizeDelegate = OnResizeDelegate();

	RegisterWindowClass(_pClassName);
	OpenWindow(_pClassName, _pWindowName, _bFullscreen);
}

CoreWindow::~CoreWindow(void)
{
	CloseWindow(m_hWindow);
}

void CoreWindow::RegisterWindowClass(const wchar_t* _pClassName)
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

void CoreWindow::OpenWindow(const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen)
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

void CoreWindow::Resize(UINT32 _width, UINT32 _height)
{
	if (m_ResizeDelegate != nullptr)
		m_ResizeDelegate(_width, _height);
}

void CoreWindow::AddOnResizeDelegate(OnResizeDelegate _pCallback)
{
	m_ResizeDelegate = _pCallback;
}

void CoreWindow::SetFullscreen(bool _bFullscreen)
{

}

void CoreWindow::SetMessageHandlerInstance(CoreWindow* _pWindow)
{
	g_pWindow = _pWindow;
}

HWND CoreWindow::GetWindowHandle(void) const
{
	return m_hWindow;
}

HINSTANCE CoreWindow::GetInstanceHandle(void) const
{
	return m_hInstance;
}

CoreWindow::Dimensions CoreWindow::GetDimensions(void) const
{
	return m_Dimensions;
}