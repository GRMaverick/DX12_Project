module;

#include <Windows.h>
#include <pix3.h>

#include "ArtemisMarkerCPU.h"
#include "ArtemisMarkerGPU.h"
#include "..\Device\CommandList.h"

module Artemis.Renderer:Profiling;

import :Device;

namespace ArtemisRenderer::Profiling
{
	ArtemisMarkerCPU::ArtemisMarkerCPU(ID3D12GraphicsCommandList* _pList, const char* _pFormatString)
	{
#if defined(_DEBUG)
		//PIXScopedEvent(_pList, PIX_COLOR_DEFAULT, _pFormatString);
#endif
	}

	ArtemisMarkerCPU::ArtemisMarkerCPU(ID3D12CommandQueue* _pQueue, const char* _pFormatString)
	{
#if defined(_DEBUG)
		//PIXScopedEvent(_pQueue, PIX_COLOR_DEFAULT, _pFormatString);
#endif
	}

	ArtemisMarkerGPU::ArtemisMarkerGPU(ArtemisRenderer::Device::CommandList* pCmdList, const char* _pFormatString)
	{
		m_pCmdList = pCmdList;
		m_pCmdList->StartMarker(_pFormatString);
	}

	ArtemisMarkerGPU::~ArtemisMarkerGPU()
	{
		m_pCmdList->EndMarker();
	}
}