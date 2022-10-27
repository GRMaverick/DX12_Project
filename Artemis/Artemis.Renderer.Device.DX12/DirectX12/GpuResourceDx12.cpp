#include "GpuResourceDx12.h"
#include "Interfaces/ICommandList.h"

#include <cassert>

namespace Artemis::Renderer::Device::Dx12
{
	GpuResourceDx12::~GpuResourceDx12( void )
	{
		if ( m_cpuBuffer )
		{
			//Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer );
			m_cpuBuffer->Release();
			delete m_cpuBuffer;
		}

		if ( m_gpuBuffer )
		{
			//Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer );
			m_gpuBuffer->Release();
			delete m_gpuBuffer;
		}
	}

	bool GpuResourceDx12::UploadResource( Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName )
	{
		HRESULT hr = S_OK;

		const UINT bufferSize = _sizeInBytes;

		const D3D12_RESOURCE_DESC   dRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize, static_cast<D3D12_RESOURCE_FLAGS>(_flags) );
		const D3D12_HEAP_PROPERTIES dHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		hr = pDevice->CreateCommittedResource( &dHeapProperties, D3D12_HEAP_FLAG_NONE, &dRdBuffer, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_gpuBuffer ) );
		if ( FAILED( hr ) )
		{
			assert( false && "Destination Buffer Setup Failed" );
			return false;
		}

		//Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::EGeometryCpu, m_cpuBuffer, bufferSize );

		if ( _pData )
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediate = nullptr;

			const D3D12_HEAP_PROPERTIES iHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
			const D3D12_RESOURCE_DESC   iRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

			hr = pDevice->CreateCommittedResource( &iHeapProperties, D3D12_HEAP_FLAG_NONE, &iRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_cpuBuffer ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Upload Buffer Setup Failed" );
				return false;
			}

			//Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::EGeometryGpu, m_cpuBuffer, bufferSize );

			D3D12_SUBRESOURCE_DATA srData = {};
			ZeroMemory( &srData, sizeof(D3D12_SUBRESOURCE_DATA) );
			srData.pData      = _pData;
			srData.RowPitch   = bufferSize;
			srData.SlicePitch = srData.RowPitch;

			_pCommandList->UpdateSubresource( m_gpuBuffer, m_cpuBuffer, 0, 0, 1, &srData );
		}

		return true;
	}

	void GpuResourceDx12::SetCpuBuffer( ID3D12Resource* _pBuffer )
	{
		m_cpuBuffer = _pBuffer;
	}

	void GpuResourceDx12::SetGpuBuffer( ID3D12Resource* _pBuffer )
	{
		m_gpuBuffer = _pBuffer;
	}

	ID3D12Resource* GpuResourceDx12::GetCpuBuffer( void ) const
	{
		return m_cpuBuffer;
	}

	ID3D12Resource* GpuResourceDx12::GetGpuBuffer( void ) const
	{
		return m_gpuBuffer;
	}
}
