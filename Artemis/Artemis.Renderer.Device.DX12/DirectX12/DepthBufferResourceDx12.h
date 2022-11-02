#pragma once

#include "GpuResourceDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	class DepthBufferResourceDx12 final : public GpuResourceDx12
	{
	public:
		DepthBufferResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned int _width, const unsigned int _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pDsvHeap, Interfaces::IDescriptorHeap* _pSrvHeap );
		~DepthBufferResourceDx12() override;
	};
}
