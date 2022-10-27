#pragma once

#include <d3d12.h>

#include "DeviceDefines.h"
#include "Interfaces\ISamplerState.h"

namespace Artemis::Renderer::Device
{
	class DescriptorHeap;

	class SamplerStateD3D12 final : public Renderer::Interfaces::ISamplerState
	{
	public:
		SamplerStateD3D12( void );
		~SamplerStateD3D12( void ) override;

		bool Initialise( ID3D12Device* _pDevice, DescriptorHeap* _pDescHeap, Renderer::Interfaces::SamplerStateFilter _eFilter, Renderer::Interfaces::SamplerStateWrapMode _eWrap, Renderer::Interfaces::SamplerStateComparisonFunction _eCompFunc );

		unsigned int       GetHeapIndex( void ) const;
		D3D12_SAMPLER_DESC GetState( void ) const;

	private:
		unsigned int       m_uiHeapIndex;
		D3D12_SAMPLER_DESC m_eSamplerState;
	};
}
