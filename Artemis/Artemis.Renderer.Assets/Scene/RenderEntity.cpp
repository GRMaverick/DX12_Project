#include "RenderEntity.h"

#include "Loading/AssimpLoader.h"

using namespace DirectX;

namespace Artemis::Renderer::Assets
{
    RenderEntity::RenderEntity() :
        m_xm3Position(),
        m_xm3Rotation(),
        m_fScale(0),
        m_RotationTheta(0),
        m_pModel(nullptr),
        m_matWorld(),
        m_pConstantBuffer(nullptr)
    {
    }

    bool RenderEntity::LoadModelFromFile(const char* _pFilename, Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCmdList)
    {
        m_pModel = nullptr;
        if (!AssimpLoader::LoadModel(_pDevice, _pCmdList, _pFilename, &m_pModel))
            return false;

        return true;
    }

    void RenderEntity::Update()
    {
        float sRotationSpeed = 10.0f * 0.016f;
        //m_RotationTheta += sRotationSpeed;

        // Update the model matrix. 
        float          angle = 25.0f;
        const XMVECTOR rotation = XMVectorSet(1, 0, 0, 0);

        const XMMATRIX s = XMMatrixScaling(m_fScale, m_fScale, m_fScale);
        const XMMATRIX r = XMMatrixRotationAxis(rotation, XMConvertToRadians(m_RotationTheta));
        const XMMATRIX t = XMMatrixTranslation(m_xm3Position.x, m_xm3Position.y, m_xm3Position.z);
        m_matWorld = s * r * t;
    }
}
