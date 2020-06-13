#pragma once
#define NOMINMAX

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "Logger.h"
#include "CLParser.h"

#define PRAGMA_TODO(todo)	__pragma(message("[TODO]: "todo));

#define BACK_BUFFERS 2

#define CONSTANT_BUFFER_SIZE(byteSize) (byteSize + 255) & ~255;

#define LogInfo_App(pFormat,...)			Logger::Log(SEVERITY_INFO, CATEGORY_APP, pFormat, __VA_ARGS__)
#define LogWarning_App(pFormat, ...)		Logger::Log(SEVERITY_WARN, CATEGORY_APP, pFormat, __VA_ARGS__)
#define LogError_App(pFormat, ...)			Logger::Log(SEVERITY_ERR, CATEGORY_APP, pFormat, __VA_ARGS__)
#define LogFatal_App(pFormat, ...)			Logger::Log(SEVERITY_FATAL, CATEGORY_APP, pFormat, __VA_ARGS__)

#define LogInfo_Renderer(pFormat, ...)		Logger::Log(SEVERITY_INFO, CATEGORY_RENDERER, pFormat, __VA_ARGS__)
#define LogWarning_Renderer(pFormat, ...)	Logger::Log(SEVERITY_WARN, CATEGORY_RENDERER, pFormat, __VA_ARGS__)
#define LogError_Renderer(pFormat, ...)		Logger::Log(SEVERITY_ERR, CATEGORY_RENDERER, pFormat, __VA_ARGS__)
#define LogFatal_Renderer(pFormat, ...)		Logger::Log(SEVERITY_FATAL, CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#define VALIDATE_D3D(test) { \
	HRESULT hr = test; \
	if(FAILED(hr)) \
	{ \
		LogError_Renderer("[HRESULT Failure]: 0x%i", hr); \
		assert(false); \
	} \
} \