#pragma once
#include <assert.h>

#include "Logging/Logger.h"
#include "Helpers/ProfileMarker.h"

#define BACK_BUFFERS 2

#define NOMINMAX

//#define LOW_LEVEL_GPU_PROFILING

#if defined(_DEBUG)
#define USE_PIX
#endif

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

using namespace Artemis::Utilities;

#if defined(LogInfo)
#undef LogInfo
#endif
#define LogInfo(pFormat, ...)		Logger::Log(SEVERITY_INFO,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogWarning)
#undef LogWarning
#endif
#define LogWarning(pFormat, ...)	Logger::Log(SEVERITY_WARN,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogError)
#undef LogError
#endif
#define LogError(pFormat, ...)		Logger::Log(SEVERITY_ERR,	CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LogFatal)
#undef LogFatal
#endif
#define LogFatal(pFormat, ...)		Logger::Log(SEVERITY_FATAL, CATEGORY_RENDERER, pFormat, __VA_ARGS__)

#if defined(LOW_LEVEL_GPU_PROFILING)
#define LOW_LEVEL_PROFILE_MARKET(pCmdList, pString) Artemis::Renderer::Helpers::RenderMarker(pCmdList, pString);
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...) Artemis::Renderer::Helpers::RenderMarker mkr(pCmdList, pFormat, __VA_ARGS__);
#else
#define LOW_LEVEL_PROFILE_MARKET(pCmdList, pString)
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...)
#endif

#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                            \
  (unsigned int)(char)(ch0)        | (unsigned int)(char)(ch1) << 8  | \
  (unsigned int)(char)(ch2) << 16  | (unsigned int)(char)(ch3) << 24   \
  )

enum DxilFourCc
{
	DFCC_Container = DXIL_FOURCC( 'D', 'X', 'B', 'C' ),
	// for back-compat with tools that look for DXBC containers
	DFCC_ResourceDef = DXIL_FOURCC( 'R', 'D', 'E', 'F' ),
	DFCC_InputSignature = DXIL_FOURCC( 'I', 'S', 'G', '1' ),
	DFCC_OutputSignature = DXIL_FOURCC( 'O', 'S', 'G', '1' ),
	DFCC_PatchConstantSignature = DXIL_FOURCC( 'P', 'S', 'G', '1' ),
	DFCC_ShaderStatistics = DXIL_FOURCC( 'S', 'T', 'A', 'T' ),
	DFCC_ShaderDebugInfoDXIL = DXIL_FOURCC( 'I', 'L', 'D', 'B' ),
	DFCC_ShaderDebugName = DXIL_FOURCC( 'I', 'L', 'D', 'N' ),
	DFCC_FeatureInfo = DXIL_FOURCC( 'S', 'F', 'I', '0' ),
	DFCC_PrivateData = DXIL_FOURCC( 'P', 'R', 'I', 'V' ),
	DFCC_RootSignature = DXIL_FOURCC( 'R', 'T', 'S', '0' ),
	DFCC_DXIL = DXIL_FOURCC( 'D', 'X', 'I', 'L' ),
	DFCC_PipelineStateValidation = DXIL_FOURCC( 'P', 'S', 'V', '0' ),
	DFCC_RuntimeData = DXIL_FOURCC( 'R', 'D', 'A', 'T' ),
	DFCC_ShaderHash = DXIL_FOURCC( 'H', 'A', 'S', 'H' ),
};
