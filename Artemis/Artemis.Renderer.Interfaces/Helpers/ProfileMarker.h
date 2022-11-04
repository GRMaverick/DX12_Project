#pragma once

#include "Interfaces/ICommandList.h"

#include <stdio.h>
#include <cstdarg>

namespace Artemis::Renderer::Helpers
{
	class RenderMarker
	{
	private:
		const Renderer::Interfaces::ICommandList* m_pCmdList = nullptr;

	public:
		RenderMarker( const Renderer::Interfaces::ICommandList* pCmdList, const char* _pFormatString )
		{
			m_pCmdList = pCmdList;
			m_pCmdList->StartMarker( _pFormatString );
		}

        template <typename... Args>
        RenderMarker(const Renderer::Interfaces::ICommandList* pCmdList, const char* _pFormatString, ...)
        {
            m_pCmdList = pCmdList;

            va_list arg;
            char    pBuffer[256];
            va_start(arg, _pFormatString);
            vsprintf_s(pBuffer, _pFormatString, arg);
            va_end(arg);

            m_pCmdList->StartMarker(pBuffer);
        }

		~RenderMarker()
		{
			m_pCmdList->EndMarker();
		}
	};
}
