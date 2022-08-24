module;

#define NO_STDIO_REDIRECT

#include "ArtemisLogger.h"

#include <stdio.h>
#include <cstdarg>

#include <Windows.h>

module Artemis.Core:Logging;

namespace ArtemisCore::Logging
{
	unsigned int ArtemisLogger::s_ActiveCategory = CATEGORY_NONE;
	unsigned int ArtemisLogger::s_ActiveSeverities = SEVERITY_INFO;

	void ArtemisLogger::SetSeverity(unsigned int _severity)
	{
		s_ActiveSeverities = _severity;
	}

	void ArtemisLogger::SetCategory(unsigned int _category)
	{
		s_ActiveCategory = _category;
	}

	void ArtemisLogger::Log(unsigned int _severity, unsigned int _category, const char* _pFormat, ...)
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