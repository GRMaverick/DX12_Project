#include "Logger.h"
#include <stdio.h>
#include <cstdarg>

#include <Windows.h>

#define NO_STDIO_REDIRECT
namespace SysUtilities
{
	unsigned int Logger::s_ActiveCategory = CATEGORY_NONE;
	unsigned int Logger::s_ActiveSeverities = SEVERITY_INFO;

	void Logger::SetSeverity(unsigned int _severity)
	{
		s_ActiveSeverities = _severity;
	}

	void Logger::SetCategory(unsigned int _category)
	{
		s_ActiveCategory = _category;
	}

	void Logger::Log(unsigned int _severity, unsigned int _category, const char* _pFormat, ...)
	{
		if (((_category & s_ActiveCategory) != 0) &&
			((_severity & s_ActiveSeverities) != 0))
		{
			va_list arg;
			char pBuffer[4096];
			va_start(arg, _pFormat);
			vsprintf_s(pBuffer, _pFormat, arg);
			va_end(arg);

			OutputDebugStringA(pBuffer);
			OutputDebugStringA("\n");
		}
	}
}