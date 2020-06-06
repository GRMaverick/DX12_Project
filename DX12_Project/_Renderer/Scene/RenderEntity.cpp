#include "Defines.h"

#include "RenderEntity.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"

#include "AssimpLoader.h"

using namespace DirectX;

bool RenderEntity::LoadModelFromFile(const char* _pFilename)
{
	CommandList* pCmdList = CommandList::Build(D3D12_COMMAND_LIST_TYPE_COPY);

	m_pModel = nullptr;
	if (!AssimpLoader::LoadModel(DeviceD3D12::Instance(), pCmdList, _pFilename, &m_pModel))
		return false;

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitToQueue(pCmdList);
	return true;
}

void RenderEntity::Update()
{
	float sRotationSpeed = 25.0f * 0.016f;
	m_RotationTheta += sRotationSpeed;

	// Update the model matrix. 
	float angle = 25.0f;
	XMVECTOR rotation = XMVectorSet(1, 0, 0, 0);
	
	XMMATRIX S = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
	XMMATRIX R = XMMatrixRotationAxis(rotation, XMConvertToRadians(m_RotationTheta));
	XMMATRIX T = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	m_World = S * R * T;
}