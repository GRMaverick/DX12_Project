#ifndef __ConstantBufferResource_h__
#define __ConstantBufferResource_h__

#include "IBufferResource.h"

#include "IShader.h"

class ConstantBufferResource : public IBufferResource
{
public:
	ConstantBufferResource(ID3D12Device* _pDevice, DescriptorHeap* _pDescHeapCBV, const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName);
	~ConstantBufferResource(void);

	bool UpdateValue(const char* _pValueName, void* _pValue, unsigned int _szValue);

	unsigned int GetHeapIndex(void) { return m_HeapIndex; }

	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(void) { return m_CpuHandle; }

private:
	unsigned int m_HeapIndex;
	char* m_pCPUMapped = nullptr;
	ConstantBufferParameters::ConstantBuffer m_ConstantParameters;

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_CpuHandle;
};
#endif // __ConstantBufferResource_h__