#ifndef __Logger_h__
#define __Logger_h__

#include <Windows.h>

const UINT SEVERITY_INFO		= 1;
const UINT SEVERITY_WARN		= SEVERITY_INFO << 1;
const UINT SEVERITY_ERR			= SEVERITY_INFO << 2;
const UINT SEVERITY_FATAL		= SEVERITY_INFO << 3;

const UINT CATEGORY_NONE		= 0;
const UINT CATEGORY_APP			= 1;
const UINT CATEGORY_RENDERER	= CATEGORY_APP << 1;

class Logger
{
public:
	static void SetSeverity(UINT _severity = SEVERITY_INFO);
	static void SetCategory(UINT _category = CATEGORY_NONE);

	static void Log(UINT _severity, UINT _category, const char* _pFormat, ...);

private:
	static UINT s_MinSeverity;
	static UINT s_ActiveCategory;
};

#endif // __Logger_h__