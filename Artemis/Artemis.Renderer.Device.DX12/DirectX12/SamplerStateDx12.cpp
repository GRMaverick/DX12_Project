#include "SamplerStateDx12.h"

#include "Helpers/d3dx12.h"

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/IDescriptorHeap.h"

namespace Artemis::Renderer::Device::Dx12
{
    SamplerStateDx12::SamplerStateDx12(void)
    {
        m_uiHeapIndex = 0;

        m_eSamplerState.MinLOD = 0;
        m_eSamplerState.MaxLOD = D3D12_FLOAT32_MAX;
        m_eSamplerState.MipLODBias = 0.0f;
        m_eSamplerState.MaxAnisotropy = 1;
    }

    SamplerStateDx12::~SamplerStateDx12(void)
    {
    }

    bool SamplerStateDx12::Initialise(const Interfaces::IGraphicsDevice* _pDevice, Interfaces::IDescriptorHeap* _pDescHeap, Interfaces::SamplerStateFilter _eFilter, Interfaces::SamplerStateWrapMode _eWrap, Interfaces::SamplerStateComparisonFunction _eCompFunc)
    {
        m_eSamplerState.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
        m_eSamplerState.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
        m_eSamplerState.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(_eWrap);
        m_eSamplerState.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(_eCompFunc);

        switch (_eFilter)
        {
        case Interfaces::SamplerStateFilter::ELinear:
            m_eSamplerState.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        default:;
        }

        m_uiHeapIndex = _pDescHeap->GetFreeIndexAndIncrement();

        ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pDescHeap->GetDeviceObject());
        CD3DX12_CPU_DESCRIPTOR_HANDLE sampHandle(pHeap->GetCPUDescriptorHandleForHeapStart());
        sampHandle.Offset(m_uiHeapIndex, _pDescHeap->GetIncrementSize());

        const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
        ID3D12Device* pDevice = const_cast<ID3D12Device*>(pConstDevice);
        pDevice->CreateSampler(&m_eSamplerState, sampHandle);

        return true;
    }

    unsigned int SamplerStateDx12::GetHeapIndex(void) const
    {
        return m_uiHeapIndex;
    }

    D3D12_SAMPLER_DESC SamplerStateDx12::GetState(void) const
    {
        return m_eSamplerState;
    }
}
