#include "Defines.h"
#include "ConstantTable.h"

#include "D3D12\Device\DeviceD3D12.h"

#include "ConstantBufferResource.h"

ConstantTable::ConstantTable(void)
{
}

ConstantTable::~ConstantTable(void)
{

}

ConstantTable* ConstantTable::Instance(void)
{
	static ConstantTable table;
	return &table;
}

extern unsigned long HashString(char* _pObject, size_t _szlength);

bool ConstantTable::CreateConstantBuffersEntries(const ConstantBufferParameters& _params)
{
    assert((m_mapConstantBuffers.size() + _params.NumberBuffers) < kMaxCBVs);

    for (unsigned int cb = 0; cb < _params.NumberBuffers; ++cb)
    {
        unsigned long ulHashName = HashString(_params.Buffers[cb].Name, strlen(_params.Buffers[cb].Name));

        if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
        {
            LogWarning_Renderer("Constant Buffer: %s already exists!", _params.Buffers[cb].Name);
        }

        m_mapConstantBuffers[ulHashName] = _params.Buffers[cb]; // DeviceD3D12::Instance()->CreateConstantBufferResource();
    }
    return true;
}

bool ConstantTable::UpdateValue(const char* _pBufferName, const char* _pValueName, void* _pValue, unsigned int _szValue)
{
    //unsigned long ulHashName = HashString((char*)_pBufferName, strlen(_pBufferName));

    //if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
    //{
    //    ConstantBufferResource* pBuffer = m_mapConstantBuffers[ulHashName];
    //    return pBuffer->UpdateValue(_pValueName, _pValue, _szValue);
    //}

    //assert("Constant Buffer Not Found");

    return false;
}

ConstantBufferResource* ConstantTable::CreateConstantBuffer(const char* _pBufferName)
{
    unsigned long ulHashName = HashString((char*)_pBufferName, strlen(_pBufferName));
    if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
    {
        return DeviceD3D12::Instance()->CreateConstantBufferResource(m_mapConstantBuffers[ulHashName]);
        //return m_mapConstantBuffers[ulHashName];
    }

    assert(false && "Constant Buffer Not Found");

    return nullptr;
}