#pragma once

#include "_Interfaces\ISamplerState.h"

#include "Defines.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class SamplerStateD3D12 final : public Interfaces::ISamplerState
		{
		public:
			SamplerStateD3D12( void );
			~SamplerStateD3D12( void ) override;

			bool Initialise( ID3D12Device* _pDevice, DescriptorHeap* _pDescHeap, SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc );

			unsigned int       GetHeapIndex( void ) const;
			D3D12_SAMPLER_DESC GetState( void ) const;

		private:
			unsigned int       m_uiHeapIndex;
			D3D12_SAMPLER_DESC m_eSamplerState;
		};
	}
}
