#include "Defines.h"
#include "ConstantTable.h"

#include "D3D12\Device\DeviceD3D12.h"

#include "ConstantBufferResource.h"

#include "SysUtilities\Hashing.h"

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

bool ConstantTable::CreateConstantBuffersEntries(const ConstantBufferParameters& _params)
{
    assert((m_mapConstantBuffers.size() + _params.NumberBuffers) < kMaxCBVs);

    for (unsigned int cb = 0; cb < _params.NumberBuffers; ++cb)
    {
        unsigned long ulHashName = Hashing::SimpleHash(_params.Buffers[cb].Name, strlen(_params.Buffers[cb].Name));

        if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
        {
            LogWarning("Constant Buffer: %s already exists!", _params.Buffers[cb].Name);
            continue;
        }

        m_mapConstantBuffers[ulHashName] = _params.Buffers[cb];
    }
    return true;
}

ConstantBufferResource* ConstantTable::CreateConstantBuffer(const char* _pBufferName)
{
    unsigned long ulHashName = Hashing::SimpleHash((char*)_pBufferName, strlen(_pBufferName));
    if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
    {
        return DeviceD3D12::Instance()->CreateConstantBufferResource(m_mapConstantBuffers[ulHashName]);
    }

    return nullptr;
}