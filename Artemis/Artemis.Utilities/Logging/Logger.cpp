#include "Logger.h"

#include <stdio.h>
#include <cstdarg>
#include <Windows.h>

#define NO_STDIO_REDIRECT

namespace Artemis::Utilities
{
	unsigned int Logger::m_sActiveCategory   = CATEGORY_NONE;
	unsigned int Logger::m_sActiveSeverities = SEVERITY_INFO;

	void Logger::SetSeverity( const unsigned int _severity )
	{
		m_sActiveSeverities = _severity;
	}

	void Logger::SetCategory( const unsigned int _category )
	{
		m_sActiveCategory = _category;
	}

	void Logger::Log( const unsigned int _severity, const unsigned int _category, const char* _pFormat, ... )
	{
		if ( ((_category & m_sActiveCategory) != 0) && ((_severity & m_sActiveSeverities) != 0) )
		{
			va_list arg;
			char    pBuffer[4096];

			size_t szOffset = 0;
			switch ( _severity )
			{
				case SEVERITY_INFO:
					OutputDebugStringA( "[INFO]: " );
					break;
				case SEVERITY_WARN:
					OutputDebugStringA( "[WARNING]: " );
					break;
				case SEVERITY_ERR:
					OutputDebugStringA( "[ERROR]: " );
					break;
				case SEVERITY_FATAL:
					OutputDebugStringA( "[FATAL]: " );
					break;
				default: ;
			}

			va_start( arg, _pFormat );
			vsprintf_s( pBuffer, _pFormat, arg );
			va_end( arg );

			OutputDebugStringA( pBuffer );
			OutputDebugStringA( "\n" );
		}
	}
}
