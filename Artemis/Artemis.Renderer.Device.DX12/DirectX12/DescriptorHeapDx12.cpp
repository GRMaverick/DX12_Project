#include "Dx12Defines.h"

#include "DescriptorHeapDx12.h"

#include <assert.h>

#include "Interfaces/IGraphicsDevice.h"

using namespace Microsoft::WRL;

namespace Artemis::Renderer::Device::Dx12
{
	DescriptorHeapDx12::DescriptorHeapDx12( void )
	{
		m_uiIncrementSize   = 0;
		m_maxDescriptors    = 0;
		m_actualDescriptors = 0;
		m_pDescriptorHeap   = nullptr;
	}

	DescriptorHeapDx12::~DescriptorHeapDx12( void )
	{
		m_uiIncrementSize   = 0;
		m_maxDescriptors    = 0;
		m_actualDescriptors = 0;

		if ( m_pDescriptorHeap )
			m_pDescriptorHeap.Reset();
	}

	void DescriptorHeapDx12::Reset()
	{
		m_uiIncrementSize   = 0;
		m_maxDescriptors    = 0;
		m_actualDescriptors = 0;

		if ( m_pDescriptorHeap )
			m_pDescriptorHeap.Reset();
	}

	bool DescriptorHeapDx12::Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::DescriptorHeapType _type, unsigned int _numDescriptors, Interfaces::DescriptorHeapFlags _flags, const wchar_t* _pDebugName )
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		ZeroMemory( &desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC) );
		desc.Type           = static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(_type);
		desc.NumDescriptors = _numDescriptors;
		desc.Flags          = static_cast<D3D12_DESCRIPTOR_HEAP_FLAGS>(_flags);

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		VALIDATE_D3D( pDevice->CreateDescriptorHeap( &desc, IID_PPV_ARGS( m_pDescriptorHeap.GetAddressOf() ) ) );
		m_uiIncrementSize = pDevice->GetDescriptorHandleIncrementSize( desc.Type );
		m_maxDescriptors  = _numDescriptors;

		m_pDescriptorHeap->SetName( _pDebugName );

		return true;
	}

	void DescriptorHeapDx12::Increment()
	{
		assert( (m_actualDescriptors <= m_maxDescriptors - 1) && "Too many SRVs allocated on this heap" );
		m_actualDescriptors++;
	}

	UINT32 DescriptorHeapDx12::GetFreeIndex( void ) const
	{
		return m_actualDescriptors;
	}

	UINT32 DescriptorHeapDx12::GetFreeIndexAndIncrement( void )
	{
		const UINT32 ret = m_actualDescriptors;
		Increment();
		return ret;
	}

	void* DescriptorHeapDx12::GetDeviceObject( void ) const
	{
		return m_pDescriptorHeap.Get();
	}

	UINT DescriptorHeapDx12::GetIncrementSize( void ) const
	{
		return m_uiIncrementSize;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapDx12::GetCpuStartHandle( void ) const
	{
		return m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapDx12::GetGpuStartHandle( void ) const
	{
		return m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	}
}
