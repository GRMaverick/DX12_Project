#pragma once

namespace ArtemisRenderer::Device
{
	class CommandList;
}

namespace ArtemisRenderer::Profiling
{
	class ArtemisMarkerGPU
	{
	private:
		ArtemisRenderer::Device::CommandList* m_pCmdList = nullptr;

	public:
		ArtemisMarkerGPU(ArtemisRenderer::Device::CommandList* pCmdList, const char* _pFormatString);

		~ArtemisMarkerGPU();
	};
}

#if defined(LOW_LEVEL_GPU_PROFILING)
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...) ArtemisRenderer::Profiling::ArtemisMarkerGPU mkr(pCmdList, pFormat, __VA_ARGS__);
#else
#define LOW_LEVEL_PROFILE_MARKER(pCmdList, pFormat, ...)
#endif