#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "GpuResourceDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	class CommandListDx12;

	class VertexBufferResourceDx12 final : public GpuResourceDx12
	{
	public:
		VertexBufferResourceDx12(const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );
		~VertexBufferResourceDx12( void ) override;

		D3D12_VERTEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_vbView;
	};
}

#endif // __VertexBufferResource_h__
