#ifndef __PixScopedEvent_h__
#define __PixScopedEvent_h__

#include <d3d12.h>
#include "pix3.h"

namespace Artemis::Renderer::Helpers
{
	class PixScopedEvent
	{
	public:
		template <typename... Args>
		PixScopedEvent( ID3D12GraphicsCommandList* _pList, const char* _pFormatString, Args ..._args )
		{
#if defined(_DEBUG)
			PIXScopedEvent( _pList, PIX_COLOR_DEFAULT, _pFormatString, _args... );
#endif
		}

		template <typename... Args>
		PixScopedEvent( ID3D12CommandQueue* _pQueue, const char* _pFormatString, Args ..._args )
		{
#if defined(_DEBUG)
			PIXScopedEvent( _pQueue, PIX_COLOR_DEFAULT, _pFormatString, _args... );
#endif
		}
	};
}

#endif //__PixScopedEvent_h__
