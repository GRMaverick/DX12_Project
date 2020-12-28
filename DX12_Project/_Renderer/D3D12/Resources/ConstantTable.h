#ifndef __ConstantTable_h__
#define __ConstantTable_h__

#include <map>

#include "ConstantBufferParameters.h"

class ConstantBufferResource;
class DescriptorHeap;

class ConstantTable
{
public:
	~ConstantTable(void);

	static ConstantTable* Instance(void);

	ConstantBufferResource* CreateConstantBuffer(const char* _pBufferName);
	bool CreateConstantBuffersEntries(const ConstantBufferParameters& _params);
	bool UpdateValue(const char* _pBufferName, const char* _pValueName, void* _pValue, unsigned int _szValue);

	DescriptorHeap* GetDescriptorHeap(void) { return m_pDescHeapCBV; }

	//ConstantBufferResource* GetConstantBuffer(const char* _pBufferName);
private:
	ConstantTable(void);

	const unsigned int kMaxCBVs = 10;
	DescriptorHeap* m_pDescHeapCBV = nullptr;
	std::map<unsigned int, ConstantBufferParameters::ConstantBuffer> m_mapConstantBuffers;
};
#endif // __ConstantTable_h__