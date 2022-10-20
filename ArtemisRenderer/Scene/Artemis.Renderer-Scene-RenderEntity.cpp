module;

module Artemis.Renderer:Scene;

import "RenderEntity.h";

import "Helpers/Defines.h";
import "Helpers/AssimpLoader.h";

import "Device/RenderDevice.h";
import "Device/CommandList.h";
import "Device/CommandQueue.h";

using namespace ArtemisRenderer::Device;
using namespace ArtemisRenderer::Helpers;

using namespace DirectX;

namespace ArtemisRenderer::Scene
{
	bool RenderEntity::LoadModelFromFile(const char* _pFilename)
	{
		CommandList* pCmdListCpy = CommandList::Build(D3D12_COMMAND_LIST_TYPE_COPY, L"CopyContext");
		pCmdListCpy->Reset();

		m_pModel = nullptr;
		if (!AssimpLoader::LoadModel(RenderDevice::Instance(), pCmdListCpy, _pFilename, &m_pModel))
			return false;

		CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitToQueue(pCmdListCpy);
		return true;
	}

	void RenderEntity::Update()
	{
		float sRotationSpeed = 10.0f * 0.016f;
		//m_RotationTheta += sRotationSpeed;

		// Update the model matrix. 
		float angle = 25.0f;
		XMVECTOR rotation = XMVectorSet(1, 0, 0, 0);

		XMMATRIX S = XMMatrixScaling(m_Scale, m_Scale, m_Scale);
		XMMATRIX R = XMMatrixRotationAxis(rotation, XMConvertToRadians(m_RotationTheta));
		XMMATRIX T = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
		m_World = S * R * T;
	}
}