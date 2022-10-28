#ifndef __UploadBuffer_h__
#define __UploadBuffer_h__

#include "Dx12Defines.h"

#include <d3d12.h>
#include <wrl.h>

namespace Artemis::Renderer::Device
{
	template <typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer( ID3D12Device* _pDevice, const UINT _elementCount, const bool _isConstantBuffer ) :
			m_bIsConstantBuffer( _isConstantBuffer )
		{
			m_uiElementByteSize = sizeof( T );

			// Constant buffer elements need to be multiples of 256 bytes.
			// This is because the hardware can only view constant data 
			// at m*256 byte offsets and of n*256 byte lengths. 
			// typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
			// UINT64 OffsetInBytes; // multiple of 256
			// UINT   SizeInBytes;   // multiple of 256
			// } D3D12_CONSTANT_BUFFER_VIEW_DESC;
			if ( _isConstantBuffer )
				m_uiElementByteSize = CONSTANT_BUFFER_SIZE( sizeof( T ) );

			const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
			const D3D12_RESOURCE_DESC   uploadResourceDesc   = CD3DX12_RESOURCE_DESC::Buffer( m_uiElementByteSize * _elementCount );
			_pDevice->CreateCommittedResource( &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &uploadResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_pUploadBuffer ) );

			m_pUploadBuffer->Map( 0, nullptr, reinterpret_cast<void**>(&m_pMappedData) );

			// We do not need to unmap until we are done with the resource.  However, we must not write to
			// the resource while it is in use by the GPU (so we must use synchronization techniques).
		}

		UploadBuffer( const UploadBuffer& _rhs )            = delete;
		UploadBuffer& operator=( const UploadBuffer& _rhs ) = delete;

		~UploadBuffer()
		{
			if ( m_pUploadBuffer != nullptr )
				m_pUploadBuffer->Unmap( 0, nullptr );

			m_pMappedData = nullptr;
		}

		ID3D12Resource* Resource() const
		{
			return m_pUploadBuffer.Get();
		}

		void CopyData( const int _elementIndex, const T& _data )
		{
			memcpy( &m_pMappedData[_elementIndex * m_uiElementByteSize], &_data, sizeof( T ) );
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pUploadBuffer;
		BYTE*                                  m_pMappedData = nullptr;

		UINT m_uiElementByteSize = 0;
		bool m_bIsConstantBuffer = false;
	};
}

#endif // __UploadBuffer_h__
