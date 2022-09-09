#ifndef __BufferResource_h__
#define __BufferResource_h__

#include <d3d12.h>

#include "../Helpers/d3dx12.h"

namespace ArtemisRenderer::Device
{
	class CommandList;
}

namespace ArtemisRenderer::Resources
{
	class IBufferResource
	{
	public:
		virtual ~IBufferResource(void);

		bool UploadResource(ID3D12Device* _pDevice, Device::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr);

		void SetCPUBuffer(ID3D12Resource* _pBuffer);
		void SetGPUBuffer(ID3D12Resource* _pBuffer);

		ID3D12Resource* GetCPUBuffer(void);
		ID3D12Resource* GetGPUBuffer(void);

	protected:
		ID3D12Resource* m_CPUBuffer;
		ID3D12Resource* m_GPUBuffer;
	};

	class IGpuBufferResource
	{
	public:
		virtual ~IGpuBufferResource(void) { }

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(void) { return m_hCpuHandle; }

	protected:
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuHandle;
	};
}

#endif // __BufferResource_h__