#include "SamplerStateD3D12.h"

#include "D3D12\Resources\DescriptorHeap.h"

namespace SysRenderer
{
	namespace D3D12
	{
		SamplerStateD3D12::SamplerStateD3D12( void )
		{
			m_uiHeapIndex = 0;

			m_eSamplerState.MinLOD        = 0;
			m_eSamplerState.MaxLOD        = D3D12_FLOAT32_MAX;
			m_eSamplerState.MipLODBias    = 0.0f;
			m_eSamplerState.MaxAnisotropy = 1;
		}

		SamplerStateD3D12::~SamplerStateD3D12( void )
		{
		}

		bool SamplerStateD3D12::Initialise( ID3D12Device* _pDevice, DescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc )
		{
			m_eSamplerState.AddressU       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
			m_eSamplerState.AddressV       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
			m_eSamplerState.AddressW       = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
			m_eSamplerState.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(_eCompFunc);

			switch ( _eFilter )
			{
				case SamplerStateFilter::ELinear:
					m_eSamplerState.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
					break;
				default: ;
			}

			m_uiHeapIndex = _pDescHeap->GetFreeIndexAndIncrement();

			CD3DX12_CPU_DESCRIPTOR_HANDLE sampHandle( _pDescHeap->GetCpuStartHandle() );
			sampHandle.Offset( m_uiHeapIndex, _pDescHeap->GetIncrementSize() );

			_pDevice->CreateSampler( &m_eSamplerState, sampHandle );

			return true;
		}

		unsigned int SamplerStateD3D12::GetHeapIndex( void ) const
		{
			return m_uiHeapIndex;
		}

		D3D12_SAMPLER_DESC SamplerStateD3D12::GetState( void ) const
		{
			return m_eSamplerState;
		}
	}
}
