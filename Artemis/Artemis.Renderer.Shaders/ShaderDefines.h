#pragma once

#include <cassert>

#include "Logging\Logger.h"

using namespace Artemis::Utilities;

#if defined(LogInfo)
#undef LogInfo
#endif
#define LogInfo(pFormat,...)			Logger::Log(SEVERITY_INFO, CATEGORY_SHADERS, pFormat, __VA_ARGS__)

#if defined(LogWarning)
#undef LogWarning
#endif
#define LogWarning(pFormat, ...)		Logger::Log(SEVERITY_WARN, CATEGORY_SHADERS, pFormat, __VA_ARGS__)

#if defined(LogError)
#undef LogError
#endif
#define LogError(pFormat, ...)			Logger::Log(SEVERITY_ERR, CATEGORY_SHADERS, pFormat, __VA_ARGS__)

#if defined(LogFatal)
#undef LogFatal
#endif
#define LogFatal(pFormat, ...)			Logger::Log(SEVERITY_FATAL, CATEGORY_SHADERS, pFormat, __VA_ARGS__)

#define VALIDATE_D3D(test) { \
	HRESULT hr = test; \
	if(FAILED(hr)) \
	{ \
		LogError("[HRESULT Failure]: 0x%i", hr); \
		assert(false); \
	} \
}