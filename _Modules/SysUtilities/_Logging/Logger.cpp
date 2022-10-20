#include "Logger.h"
#include <stdio.h>
#include <cstdarg>

#include <Windows.h>

#define NO_STDIO_REDIRECT

namespace SysUtilities
{
	unsigned int Logger::s_ActiveCategory   = CATEGORY_NONE;
	unsigned int Logger::s_ActiveSeverities = SEVERITY_INFO;

	void Logger::SetSeverity( const unsigned int _severity )
	{
		s_ActiveSeverities = _severity;
	}

	void Logger::SetCategory( const unsigned int _category )
	{
		s_ActiveCategory = _category;
	}

	void Logger::Log( const unsigned int _severity, const unsigned int _category, const char* _pFormat, ... )
	{
		if ( ((_category & s_ActiveCategory) != 0) && ((_severity & s_ActiveSeverities) != 0) )
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
