#include "Logger.h"
#include <stdio.h>

UINT Logger::s_ActiveCategory = CATEGORY_NONE;
UINT Logger::s_MinSeverity = SEVERITY_INFO;

void Logger::SetSeverity(UINT _severity)
{
	s_MinSeverity = _severity;
}

void Logger::SetCategory(UINT _category)
{
	s_ActiveCategory = _category;
}

void Logger::Log(UINT _severity, UINT _category, const char* _pFormat, ...)
{
	if (((_category & s_ActiveCategory) != 0) &&
		(_severity >= s_MinSeverity))
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