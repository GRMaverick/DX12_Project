#pragma once
#include "Interfaces/IBufferResource.h"

namespace Artemis::Renderer::Device::Dx12
{
	class GpuResourceDx12 : public Interfaces::IGpuResource
	{
	public:
		~GpuResourceDx12( void ) override;

		bool UploadResource( Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = nullptr );

		bool UpdateValue( const char* _pValueName, const void* _pValue, const unsigned int _szValue ) const override
		{
			return false;
		}

		void SetCpuBuffer( ID3D12Resource* _pBuffer );
		void SetGpuBuffer( ID3D12Resource* _pBuffer );

		ID3D12Resource*               GetCpuBuffer( void ) const;
		ID3D12Resource*               GetGpuBuffer( void ) const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle( void ) const { return m_hCpuHandle; }

	protected:
		ID3D12Resource*               m_cpuBuffer  = nullptr;
		ID3D12Resource*               m_gpuBuffer  = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuHandle = {};
	};
}
