#ifndef __BufferResource_h__
#define __BufferResource_h__

#include <d3d12.h>
#include "D3D12\d3dx12.h"

#include <wrl.h>

namespace SysRenderer
{
	namespace D3D12
	{
		class CommandList;
	}

	namespace Interfaces
	{
		class IBufferResource
		{
		public:
			virtual ~IBufferResource( void );

			bool UploadResource( ID3D12Device* _pDevice, D3D12::CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );

			void SetCpuBuffer( Microsoft::WRL::ComPtr<ID3D12Resource> _pBuffer );
			void SetGpuBuffer( Microsoft::WRL::ComPtr<ID3D12Resource> _pBuffer );

			Microsoft::WRL::ComPtr<ID3D12Resource> GetCpuBuffer( void ) const;
			Microsoft::WRL::ComPtr<ID3D12Resource> GetGpuBuffer( void ) const;

		protected:
			Microsoft::WRL::ComPtr<ID3D12Resource> m_cpuBuffer;
			Microsoft::WRL::ComPtr<ID3D12Resource> m_gpuBuffer;
		};

		class IGpuBufferResource
		{
		public:
			virtual ~IGpuBufferResource( void )
			{
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle( void ) const { return m_hCpuHandle; }

		protected:
			CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuHandle = {};
		};
	}
}

#endif // __BufferResource_h__
