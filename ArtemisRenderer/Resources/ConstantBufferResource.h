#ifndef __ConstantBufferResource_h__
#define __ConstantBufferResource_h__

#include "IBufferResource.h"

#include "ConstantBufferParameters.h"

namespace ArtemisRenderer::Resources
{
	class DescriptorHeap;

	class ConstantBufferResource : public IBufferResource, public IGpuBufferResource
	{
	public:
		ConstantBufferResource(ID3D12Device* _pDevice, DescriptorHeap* _pDescHeapCBV, const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName);
		~ConstantBufferResource(void);

		bool UpdateValue(const char* _pValueName, void* _pValue, unsigned int _szValue);

		unsigned int GetHeapIndex(void) { return m_HeapIndex; }
		unsigned int GetBindPoint(void) { return m_ConstantParameters.BindPoint; }
	private:
		unsigned int m_HeapIndex;
		char* m_pCPUMapped = nullptr;
		ConstantBufferParameters::ConstantBuffer m_ConstantParameters;
	};
}

#endif // __ConstantBufferResource_h__