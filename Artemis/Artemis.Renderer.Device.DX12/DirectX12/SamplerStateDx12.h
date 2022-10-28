#pragma once

#include <d3d12.h>

#include "Dx12Defines.h"
#include "Interfaces\ISamplerState.h"

namespace Artemis::Renderer::Device::Dx12
{
	class SamplerStateDx12 final : public Interfaces::ISamplerState
	{
	public:
		SamplerStateDx12( void );
		~SamplerStateDx12( void ) override;

		bool Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::IDescriptorHeap* _pDescHeap, Interfaces::SamplerStateFilter _eFilter, Interfaces::SamplerStateWrapMode _eWrap, Interfaces::SamplerStateComparisonFunction _eCompFunc ) override;

		unsigned int       GetHeapIndex( void ) const;
		D3D12_SAMPLER_DESC GetState( void ) const;

	private:
		unsigned int       m_uiHeapIndex;
		D3D12_SAMPLER_DESC m_eSamplerState;
	};
}
