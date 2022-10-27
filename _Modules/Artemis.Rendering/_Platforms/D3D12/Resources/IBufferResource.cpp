#include "D3D12\Device\CommandList.h"

#include "D3D12\Resources\IBufferResource.h"

#include "SysMemory/include/MemoryGlobalTracking.h"

#include <assert.h>

using namespace Microsoft::WRL;

using namespace SysMemory;

namespace SysRenderer
{
	using namespace D3D12;

	namespace Interfaces
	{
		IBufferResource::~IBufferResource( void )
		{
			if ( m_cpuBuffer )
			{
				MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer.Get() );
				m_cpuBuffer.Reset();
			}

			if ( m_gpuBuffer )
			{
				MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer.Get() );
				m_gpuBuffer.Reset();
			}
		}

		bool IBufferResource::UploadResource( ID3D12Device* _pDevice, CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName )
		{
			HRESULT hr = S_OK;

			const UINT bufferSize = _sizeInBytes;

			const D3D12_RESOURCE_DESC   dRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize, _flags );
			const D3D12_HEAP_PROPERTIES dHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );

			hr = _pDevice->CreateCommittedResource( &dHeapProperties, D3D12_HEAP_FLAG_NONE, &dRdBuffer, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( m_gpuBuffer.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Destination Buffer Setup Failed" );
				return false;
			}

			MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::EGeometryCpu, m_cpuBuffer.Get(), bufferSize );

			if ( _pData )
			{
				ComPtr<ID3D12Resource> pIntermediate = nullptr;

				const D3D12_HEAP_PROPERTIES iHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
				const D3D12_RESOURCE_DESC   iRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

				hr = _pDevice->CreateCommittedResource( &iHeapProperties, D3D12_HEAP_FLAG_NONE, &iRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( m_cpuBuffer.GetAddressOf() ) );
				if ( FAILED( hr ) )
				{
					assert( false && "Upload Buffer Setup Failed" );
					return false;
				}

				MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory::EGeometryGpu, m_cpuBuffer.Get(), bufferSize );

				D3D12_SUBRESOURCE_DATA srData = {};
				ZeroMemory( &srData, sizeof(D3D12_SUBRESOURCE_DATA) );
				srData.pData      = _pData;
				srData.RowPitch   = bufferSize;
				srData.SlicePitch = srData.RowPitch;

				_pCommandList->UpdateSubresource( m_gpuBuffer.Get(), m_cpuBuffer.Get(), 0, 0, 1, &srData );
			}

			return true;
		}

		void IBufferResource::SetCpuBuffer( const ComPtr<ID3D12Resource> _pBuffer )
		{
			m_cpuBuffer = _pBuffer;
		}

		void IBufferResource::SetGpuBuffer( const ComPtr<ID3D12Resource> _pBuffer )
		{
			m_gpuBuffer = _pBuffer;
		}

		ComPtr<ID3D12Resource> IBufferResource::GetCpuBuffer( void ) const
		{
			return m_cpuBuffer;
		}

		ComPtr<ID3D12Resource> IBufferResource::GetGpuBuffer( void ) const
		{
			return m_gpuBuffer;
		}
	}
}
