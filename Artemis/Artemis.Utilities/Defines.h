#pragma once

#include "Logging/Logger.h"

#if defined(LogInfo)
#undef LogInfo
#endif

#if defined(LogWarning)
#undef LogWarning
#endif

#if defined(LogError)
#undef LogError
#endif

#if defined(LogFatal)
#undef LogFatal
#endif

#if !defined(ARRAYSIZE)
#define ARRAYSIZE(a) ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

#define LogInfo(pFormat,...)			Logger::Log(SEVERITY_INFO,	CATEGORY_UTILITIES, pFormat, __VA_ARGS__)
#define LogWarning(pFormat, ...)		Logger::Log(SEVERITY_WARN,	CATEGORY_UTILITIES, pFormat, __VA_ARGS__)
#define LogError(pFormat, ...)			Logger::Log(SEVERITY_ERR,	CATEGORY_UTILITIES, pFormat, __VA_ARGS__)
#define LogFatal(pFormat, ...)			Logger::Log(SEVERITY_FATAL, CATEGORY_UTILITIES, pFormat, __VA_ARGS__)
