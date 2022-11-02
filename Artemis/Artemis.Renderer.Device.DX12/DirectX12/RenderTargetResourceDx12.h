#pragma once

#include "GpuResourceDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	class RenderTargetResourceDx12 final : public GpuResourceDx12
	{
	public:
		RenderTargetResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned int _width, const unsigned int _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pRtvHeap, Interfaces::IDescriptorHeap* _pSrvHeap );
		~RenderTargetResourceDx12() override;
	};
}
