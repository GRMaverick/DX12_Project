module;

#include <Windows.h>
#include <pix3.h>

#include "ArtemisMarkerCPU.h"

module Artemis.Renderer:Profiling;

namespace ArtemisRenderer::Profiling
{
	template<typename... Args>
	ArtemisMarkerCPU::ArtemisMarkerCPU(ID3D12GraphicsCommandList* _pList, const char* _pFormatString, Args... _args)
	{
#if defined(_DEBUG)
		PIXScopedEvent(_pList, PIX_COLOR_DEFAULT, _pFormatString, _args...);
#endif
	}

	template<typename... Args>
	ArtemisMarkerCPU::ArtemisMarkerCPU(ID3D12CommandQueue* _pQueue, const char* _pFormatString, Args... _args)
	{
#if defined(_DEBUG)
		PIXScopedEvent(_pQueue, PIX_COLOR_DEFAULT, _pFormatString, _args...);
#endif
	}
}