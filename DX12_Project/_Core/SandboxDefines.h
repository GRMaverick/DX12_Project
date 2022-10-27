#pragma once
#define NOMINMAX

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "Logging\Logger.h"

#if defined(LogInfo)
#undef LogInfo
#endif
#define LogInfo(pFormat,...)			Logger::Log(SEVERITY_INFO, CATEGORY_APP, pFormat, __VA_ARGS__)

#if defined(LogWarning)
#undef LogWarning
#endif
#define LogWarning(pFormat, ...)		Logger::Log(SEVERITY_WARN, CATEGORY_APP, pFormat, __VA_ARGS__)

#if defined(LogError)
#undef LogError
#endif
#define LogError(pFormat, ...)			Logger::Log(SEVERITY_ERR, CATEGORY_APP, pFormat, __VA_ARGS__)

#if defined(LogFatal)
#undef LogFatal
#endif
#define LogFatal(pFormat, ...)			Logger::Log(SEVERITY_FATAL, CATEGORY_APP, pFormat, __VA_ARGS__)

#define PRAGMA_TODO(todo)	__pragma(message("[TODO]: "todo));
