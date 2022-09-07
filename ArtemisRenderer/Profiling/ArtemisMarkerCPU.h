
#ifndef __PixScopedEvent_h__
#define __PixScopedEvent_h__

import <d3d12.h>;

namespace ArtemisRenderer::Profiling
{
	class ArtemisMarkerCPU
	{
	public:
		ArtemisMarkerCPU(ID3D12GraphicsCommandList* _pList, const char* _pFormatString);
		ArtemisMarkerCPU(ID3D12CommandQueue* _pQueue, const char* _pFormatString);
	};
}

#endif //__PixScopedEvent_h__