#ifndef __IndexBufferResource_h__
#define __IndexBufferResource_h__

#include "GpuResourceDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	class CommandListDx12;

	class IndexBufferResourceDx12 final : public GpuResourceDx12
	{
	public:
		IndexBufferResourceDx12(void) = delete;
		IndexBufferResourceDx12(const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );
		~IndexBufferResourceDx12( void ) override;

		D3D12_INDEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_INDEX_BUFFER_VIEW m_ibView;
	};
}

#endif // __IndexBufferResource_h__
