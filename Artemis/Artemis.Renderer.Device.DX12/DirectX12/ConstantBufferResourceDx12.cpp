#include "Dx12Defines.h"

#include "Helpers\d3dx12.h"

#include "ConstantBufferResourceDx12.h"

#define CONSTANT_BUFFER_SIZE(byteSize) (byteSize + 255) & ~255;

namespace Artemis::Renderer::Device::Dx12
{
	ConstantBufferResourceDx12::ConstantBufferResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::IDescriptorHeap* _pDescHeapCbv, const Artemis::Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName )
	{
		// Constant buffer elements need to be multiples of 256 bytes.
		// This is because the hardware can only view constant data 
		// at m*256 byte offsets and of n*256 byte lengths. 
		// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
		// UINT64 OffsetInBytes; // multiple of 256
		// UINT   SizeInBytes;   // multiple of 256
		// } D3D12_CONSTANT_BUFFER_VIEW_DESC;
		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		m_cbpConstantParameters = _params;

		m_CbvHeapIndex = _pDescHeapCbv->GetFreeIndexAndIncrement();

		const unsigned int alignedSize = CONSTANT_BUFFER_SIZE( m_cbpConstantParameters.Size );

		const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
		const D3D12_RESOURCE_DESC   uploadResourceDesc   = CD3DX12_RESOURCE_DESC::Buffer( alignedSize );
		pDevice->CreateCommittedResource( &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_gpuBuffer ) );

		m_gpuBuffer->Map( 0, nullptr, reinterpret_cast<void**>(&m_pCpuMapped) );

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation                  = m_gpuBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes                     = alignedSize;

		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pDescHeapCbv->GetDeviceObject());
		m_hCbvCpuHandle             = CD3DX12_CPU_DESCRIPTOR_HANDLE( pHeap->GetCPUDescriptorHandleForHeapStart() );
		m_hCbvCpuHandle.Offset( m_CbvHeapIndex, _pDescHeapCbv->GetIncrementSize() );

		pDevice->CreateConstantBufferView( &cbvDesc, m_hCbvCpuHandle );

		// We do not need to unmap until we are done with the resource.  However, we must not write to
		// the resource while it is in use by the GPU (so we must use synchronization techniques).
	}

	ConstantBufferResourceDx12::~ConstantBufferResourceDx12( void )
	{
		if ( m_gpuBuffer != nullptr )
			m_gpuBuffer->Unmap( 0, nullptr );

		m_pCpuMapped = nullptr;
	}

	bool ConstantBufferResourceDx12::UpdateValue( const char* _pValueName, const void* _pValue, const unsigned int _szValue ) const
	{
		if ( m_cbpConstantParameters.Size == _szValue )
		{
			memcpy( m_pCpuMapped, _pValue, _szValue );
			return true;
		}

		//PRAGMA_TODO( "Variable Specific Updates" );
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
