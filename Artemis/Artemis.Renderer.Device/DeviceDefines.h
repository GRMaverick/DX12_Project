#pragma once

#include "Logging/Logger.h"

#define BACK_BUFFERS 2

#define NOMINMAX

//#define LOW_LEVEL_GPU_PROFILING

#define PRAGMA_TODO(todo)	__pragma(message("[TODO]: "todo));
#define CONSTANT_BUFFER_SIZE(byteSize) (byteSize + 255) & ~255;

#define VALIDATE_D3D(test) { \
	HRESULT hr = test; \
	if(FAILED(hr)) \
	{ \
		LogError("[HRESULT Failure]: 0x%i", hr); \
		assert(false); \
	} \
}

#if defined(LogInfo)
#undef LogInfo
#endif
#define LogInfo(pFormat, ...)		Artemis::Utilities::Logger::Log(SEVERITY_INFO,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogWarning)
#undef LogWarning
#endif
#define LogWarning(pFormat, ...)	Artemis::Utilities::Logger::Log(SEVERITY_WARN,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogError)
#undef LogError
#endif
#define LogError(pFormat, ...)		Artemis::Utilities::Logger::Log(SEVERITY_ERR,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogFatal)
#undef LogFatal
#endif
#define LogFatal(pFormat, ...)		Artemis::Utilities::Logger::Log(SEVERITY_FATAL, CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LOW_LEVEL_GPU_PROFILING)
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...) RenderMarker mkr(pCmdList, pFormat, __VA_ARGS__);
#else
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...)
#endif
