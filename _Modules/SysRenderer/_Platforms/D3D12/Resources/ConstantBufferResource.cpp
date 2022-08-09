#include "Defines.h"

#include "D3D12\d3dx12.h"


#include "DescriptorHeap.h"
#include "ConstantBufferResource.h"

namespace SysRenderer
{
    namespace D3D12
    {
        ConstantBufferResource::ConstantBufferResource(ID3D12Device* _pDevice, DescriptorHeap* _pDescHeapCBV, const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName)
        {
            // Constant buffer elements need to be multiples of 256 bytes.
            // This is because the hardware can only view constant data 
            // at m*256 byte offsets and of n*256 byte lengths. 
            // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
            // UINT64 OffsetInBytes; // multiple of 256
            // UINT   SizeInBytes;   // multiple of 256
            // } D3D12_CONSTANT_BUFFER_VIEW_DESC;

            m_ConstantParameters = _params;

            m_HeapIndex = _pDescHeapCBV->GetFreeIndexAndIncrement();

            unsigned int alignedSize = CONSTANT_BUFFER_SIZE(m_ConstantParameters.Size);

            D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            D3D12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedSize);
            _pDevice->CreateCommittedResource(
                &uploadHeapProperties,
                D3D12_HEAP_FLAG_NONE,
                &uploadResourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_GPUBuffer));

            m_GPUBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pCPUMapped));

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = m_GPUBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = alignedSize;

            m_hCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pDescHeapCBV->GetCPUStartHandle());
            m_hCpuHandle.Offset(m_HeapIndex, _pDescHeapCBV->GetIncrementSize());

            _pDevice->CreateConstantBufferView(&cbvDesc, m_hCpuHandle);

            // We do not need to unmap until we are done with the resource.  However, we must not write to
            // the resource while it is in use by the GPU (so we must use synchronization techniques).
        }

        ConstantBufferResource::~ConstantBufferResource(void)
        {
            if (m_GPUBuffer != nullptr)
                m_GPUBuffer->Unmap(0, nullptr);

            m_pCPUMapped = nullptr;
        }

        bool ConstantBufferResource::UpdateValue(const char* _pValueName, void* _pValue, unsigned int _szValue)
        {
            if (m_ConstantParameters.Size == _szValue)
            {
                memcpy(m_pCPUMapped, _pValue, _szValue);
                return true;
            }

            PRAGMA_TODO("Variable Specific Updates");
            //for (unsigned int i = 0; i < m_ConstantParameters.NumberVariables; ++i)
            //{
            //    ConstantBufferParameters::Variable& variable = m_ConstantParameters.Variables[i];
            //    if (strncmp(_pValueName, variable.Name, ARRAYSIZE(variable.Name)))
            //    {
            //        memcpy(&m_pCPUMapped[variable.Offset], _pValue, _szValue);
            //    }
            //}
            return false;
        }
    }
}