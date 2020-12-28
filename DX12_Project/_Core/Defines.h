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

//#define VALIDATE_D3D(test) test

#define VALIDATE_D3D(test) { \
	HRESULT hr = test; \
	if(FAILED(hr)) \
	{ \
		LogError_Renderer("[HRESULT Failure]: 0x%i", hr); \
		assert(false); \
	} \
} \

#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
  (uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24   \
  )

enum DxilFourCC {
	DFCC_Container = DXIL_FOURCC('D', 'X', 'B', 'C'), // for back-compat with tools that look for DXBC containers
	DFCC_ResourceDef = DXIL_FOURCC('R', 'D', 'E', 'F'),
	DFCC_InputSignature = DXIL_FOURCC('I', 'S', 'G', '1'),
	DFCC_OutputSignature = DXIL_FOURCC('O', 'S', 'G', '1'),
	DFCC_PatchConstantSignature = DXIL_FOURCC('P', 'S', 'G', '1'),
	DFCC_ShaderStatistics = DXIL_FOURCC('S', 'T', 'A', 'T'),
	DFCC_ShaderDebugInfoDXIL = DXIL_FOURCC('I', 'L', 'D', 'B'),
	DFCC_ShaderDebugName = DXIL_FOURCC('I', 'L', 'D', 'N'),
	DFCC_FeatureInfo = DXIL_FOURCC('S', 'F', 'I', '0'),
	DFCC_PrivateData = DXIL_FOURCC('P', 'R', 'I', 'V'),
	DFCC_RootSignature = DXIL_FOURCC('R', 'T', 'S', '0'),
	DFCC_DXIL = DXIL_FOURCC('D', 'X', 'I', 'L'),
	DFCC_PipelineStateValidation = DXIL_FOURCC('P', 'S', 'V', '0'),
	DFCC_RuntimeData = DXIL_FOURCC('R', 'D', 'A', 'T'),
	DFCC_ShaderHash = DXIL_FOURCC('H', 'A', 'S', 'H'),
};