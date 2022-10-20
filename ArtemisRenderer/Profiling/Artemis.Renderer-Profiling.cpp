module;

//#include <pix3.h>
//#include <Windows.h>

module Artemis.Renderer:Profiling;

import "ArtemisMarkerCPU.h";
import "ArtemisMarkerGPU.h";
import "Device/CommandList.h";

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