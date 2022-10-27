#include "DeviceDefines.h"

#include "Helpers\d3dx12.h"

#include "DescriptorHeap.h"
#include "ConstantBufferResource.h"

namespace Artemis::Renderer::Device
{
	ConstantBufferResource::ConstantBufferResource( Interfaces::IGraphicsDevice* _pDevice, Interfaces::IDescriptorHeap* _pDescHeapCbv, const Artemis::Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName )
	{
		// Constant buffer elements need to be multiples of 256 bytes.
		// This is because the hardware can only view constant data 
		// at m*256 byte offsets and of n*256 byte lengths. 
		// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
		// UINT64 OffsetInBytes; // multiple of 256
		// UINT   SizeInBytes;   // multiple of 256
		// } D3D12_CONSTANT_BUFFER_VIEW_DESC;

		ID3D12Device* pDevice = static_cast<ID3D12Device*>(_pDevice->GetDeviceObject());

		m_cbpConstantParameters = _params;
		m_uiHeapIndex           = _pDescHeapCbv->GetFreeIndexAndIncrement();

		const unsigned int alignedSize = CONSTANT_BUFFER_SIZE( m_cbpConstantParameters.Size );

		const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
		const D3D12_RESOURCE_DESC   uploadResourceDesc   = CD3DX12_RESOURCE_DESC::Buffer( alignedSize );
		_pDevice->CreateCommittedResource( &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_gpuBuffer ) );

		m_gpuBuffer->Map( 0, nullptr, reinterpret_cast<void**>(&m_pCpuMapped) );

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation                  = m_gpuBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes                     = alignedSize;

		m_hCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE( _pDescHeapCbv->GetCpuStartHandle() );
		m_hCpuHandle.Offset( m_uiHeapIndex, _pDescHeapCbv->GetIncrementSize() );

		_pDevice->CreateConstantBufferView( &cbvDesc, m_hCpuHandle );

		// We do not need to unmap until we are done with the resource.  However, we must not write to
		// the resource while it is in use by the GPU (so we must use synchronization techniques).
	}

	ConstantBufferResource::~ConstantBufferResource( void )
	{
		if ( m_gpuBuffer != nullptr )
			m_gpuBuffer->Unmap( 0, nullptr );

		m_pCpuMapped = nullptr;
	}

	bool ConstantBufferResource::UpdateValue( const char* _pValueName, const void* _pValue, const unsigned int _szValue ) const
	{
		if ( m_cbpConstantParameters.Size == _szValue )
		{
			memcpy( m_pCpuMapped, _pValue, _szValue );
			return true;
		}

		PRAGMA_TODO( "Variable Specific Updates" );
		//for (unsigned int i = 0; i < m_cbpConstantParameters.NumberVariables; ++i)
		//{
		//    ConstantBufferParameters::Variable& variable = m_cbpConstantParameters.Variables[i];
		//    if (strncmp(_pValueName, variable.Name, ARRAYSIZE(variable.Name)))
		//    {
		//        memcpy(&m_pCPUMapped[variable.Offset], _pValue, _szValue);
		//    }
		//}
		return false;
	}
}
