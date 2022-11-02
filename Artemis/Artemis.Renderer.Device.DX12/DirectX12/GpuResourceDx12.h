#pragma once
#include "Interfaces/IBufferResource.h"

namespace Artemis::Renderer::Device::Dx12
{
	class GpuResourceDx12 : public Interfaces::IGpuResource
	{
	public:
		~GpuResourceDx12( void ) override;

		bool UpdateValue( const char* _pValueName, const void* _pValue, const unsigned int _szValue ) const override
		{
			return false;
		}

		void SetCpuBuffer( ID3D12Resource* _pBuffer );
		void SetGpuBuffer( ID3D12Resource* _pBuffer );

		ID3D12Resource* GetCpuBuffer( void ) const;
		ID3D12Resource* GetGpuBuffer( void ) const;

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCbvCpuHandle( void ) const { return m_hCbvCpuHandle; }
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle( void ) const { return m_hSrvCpuHandle; }
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle( void ) const { return m_hRtvCpuHandle; }
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle( void ) const { return m_hDsvCpuHandle; }

		UINT32 GetCbvHeapIndex( void ) const { return m_CbvHeapIndex; }
		UINT32 GetSrvHeapIndex( void ) const { return m_SrvHeapIndex; }
		UINT32 GetRtvHeapIndex( void ) const { return m_RtvHeapIndex; }
		UINT32 GetDsvHeapIndex( void ) const { return m_DsvHeapIndex; }

		static unsigned long GetMemoryRequirement( unsigned int _width, unsigned int _height, unsigned int _depth, DXGI_FORMAT _format );

	protected:
		void BuildShaderResourceView( ID3D12Device* _pDevice, Interfaces::IDescriptorHeap* _pSrvHeap );
		bool UploadResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );

		ID3D12Resource* m_cpuBuffer = nullptr;
		ID3D12Resource* m_gpuBuffer = nullptr;

		UINT                          m_CbvHeapIndex  = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCbvCpuHandle = {};

		UINT                          m_SrvHeapIndex  = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hSrvCpuHandle = {};

		UINT                          m_RtvHeapIndex  = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hRtvCpuHandle = {};

		UINT                          m_DsvHeapIndex  = 0;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hDsvCpuHandle = {};
	};
}
