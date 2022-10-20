module;

module Artemis.Renderer:Resources;

import "ConstantTable.h";

import "ConstantBuffer.h";

import "Helpers/Defines.h";
import "Device/RenderDevice.h";

using namespace ArtemisRenderer::Device;

import Artemis.Core;

using namespace ArtemisCore::Hashing;

namespace ArtemisRenderer::Resources
{
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
            unsigned long long ulHashName = SimpleHash(_params.Buffers[cb].Name, strlen(_params.Buffers[cb].Name)).Hash;

            if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
            {
                LogWarning("Constant Buffer: %s already exists!", _params.Buffers[cb].Name);
                continue;
            }

            m_mapConstantBuffers[ulHashName] = _params.Buffers[cb];
        }
        return true;
    }

    IBufferResource* ConstantTable::CreateConstantBuffer(const char* _pBufferName)
    {
        unsigned long long ulHashName = SimpleHash((char*)_pBufferName, strlen(_pBufferName)).Hash;
        if (m_mapConstantBuffers.find(ulHashName) != m_mapConstantBuffers.end())
        {
            return RenderDevice::Instance()->CreateConstantBufferResource(m_mapConstantBuffers[ulHashName]);
        }

        LogError("Constant Buffer: %s - Does not exist in Hash Map", _pBufferName);
        return nullptr;
    }
}