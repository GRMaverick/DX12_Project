#include "Defines.h"

#include "DescriptorHeap.h"

#include <assert.h>

using namespace Microsoft::WRL;

using namespace SysUtilities;

namespace SysRenderer
{
	namespace D3D12
	{
		DescriptorHeap::DescriptorHeap( void )
		{
			m_uiIncrementSize   = 0;
			m_maxDescriptors    = 0;
			m_actualDescriptors = 0;
			m_pDescriptorHeap   = nullptr;
		}

		DescriptorHeap::~DescriptorHeap( void )
		{
			m_uiIncrementSize   = 0;
			m_maxDescriptors    = 0;
			m_actualDescriptors = 0;

			if ( m_pDescriptorHeap )
				m_pDescriptorHeap.Reset();
		}

		bool DescriptorHeap::Initialise( const ComPtr<ID3D12Device> _pDevice, const D3D12_DESCRIPTOR_HEAP_TYPE _type, const UINT _numDescriptors, const D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName )
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			ZeroMemory( &desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC) );
			desc.Type           = _type;
			desc.NumDescriptors = _numDescriptors;
			desc.Flags          = _flags;

			VALIDATE_D3D( _pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pDescriptorHeap.GetAddressOf())) );
			m_uiIncrementSize = _pDevice->GetDescriptorHandleIncrementSize( _type );
			m_maxDescriptors  = _numDescriptors;

			m_pDescriptorHeap->SetName( _pDebugName );

			return true;
		}

		void DescriptorHeap::Increment()
		{
			assert( (m_actualDescriptors <= m_maxDescriptors - 1) && "Too many SRVs allocated on this heap" );
			m_actualDescriptors++;
		}

		UINT32 DescriptorHeap::GetFreeIndex( void ) const
		{
			return m_actualDescriptors;
		}

		UINT32 DescriptorHeap::GetFreeIndexAndIncrement( void )
		{
			const UINT32 ret = m_actualDescriptors;
			Increment();
			return ret;
		}

		ID3D12DescriptorHeap* DescriptorHeap::GetHeap( void ) const
		{
			return m_pDescriptorHeap.Get();
		}

		UINT DescriptorHeap::GetIncrementSize( void ) const
		{
			return m_uiIncrementSize;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuStartHandle( void ) const
		{
			return m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		}

		D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuStartHandle( void ) const
		{
			return m_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		}
	}
}
