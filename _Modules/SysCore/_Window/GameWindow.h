#ifndef __CoreWindow_h__
#define __CoreWindow_h__

#include <Windows.h>
#include <functional>

namespace SysCore
{
	typedef std::function<void(UINT32, UINT32)> OnResizeDelegate;

	class GameWindow
	{
	public:
		struct Dimensions
		{
			LONG WindowWidth = 0;
			LONG WindowHeight = 0;
			LONG ScreenWidth = 0;
			LONG ScreenHeight = 0;
			BOOL IsFullscreen = 0;
		};

		GameWindow(HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen = false);
		~GameWindow(void);

		static void SetMessageHandlerInstance(GameWindow* _pWindow);

		LRESULT MessageHandler(HWND _hwnd, UINT _umsg, WPARAM _wparam, LPARAM _lparam);

		void Resize(UINT32 _width, UINT32 _height);
		void AddOnResizeDelegate(OnResizeDelegate _pCallback);

		void			SetFullscreen(bool _bFullscreen);

		HWND			GetWindowHandle(void) const;
		HINSTANCE		GetInstanceHandle(void) const;
		Dimensions		GetDimensions(void) const;

	private:
		HWND				m_hWindow;
		HINSTANCE			m_hInstance;
		Dimensions			m_Dimensions;

		OnResizeDelegate	m_ResizeDelegate = nullptr;


		void RegisterWindowClass(const wchar_t* _pClassName);
		void OpenWindow(const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen);
	};
}

#endif // __CoreWindow_h__