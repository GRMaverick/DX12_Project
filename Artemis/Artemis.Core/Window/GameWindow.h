#ifndef __CoreWindow_h__
#define __CoreWindow_h__

#include <Windows.h>
#include <functional>

namespace Artemis::Core
{
	typedef std::function<void( UINT32, UINT32 )> OnResizeDelegate;

	class GameWindow
	{
	public:
		struct Dimensions
		{
			LONG WindowWidth  = 0;
			LONG WindowHeight = 0;
			LONG ScreenWidth  = 0;
			LONG ScreenHeight = 0;
			BOOL IsFullscreen = 0;
		};

		GameWindow( void );
		GameWindow( const GameWindow& _rhs );
		GameWindow( HINSTANCE _hInstance, const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen = false );
		~GameWindow( void );

		static void SetMessageHandlerInstance( GameWindow* _pWindow );

		LRESULT MessageHandler( const HWND _hwnd, const UINT _umsg, const WPARAM _wparam, const LPARAM _lparam ) const;

		void Resize( const UINT32 _width, const UINT32 _height ) const;
		void AddOnResizeDelegate( OnResizeDelegate _pCallback );

		void SetFullscreen( bool _bFullscreen ) const;

		HWND       GetWindowHandle( void ) const;
		HINSTANCE  GetInstanceHandle( void ) const;
		Dimensions GetDimensions( void ) const;

	private:
		HWND       m_HWindow;
		HINSTANCE  m_HInstance;
		Dimensions m_Dimensions;

		OnResizeDelegate m_dResizeDelegate = nullptr;

		void RegisterWindowClass( const wchar_t* _pClassName ) const;
		void OpenWindow( const wchar_t* _pClassName, const wchar_t* _pWindowName, bool _bFullscreen );
	};
}

#endif // __CoreWindow_h__
