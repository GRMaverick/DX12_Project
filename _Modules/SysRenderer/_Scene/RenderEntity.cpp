#include "Defines.h"

#include "RenderEntity.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"

#include "AssimpLoader.h"

using namespace DirectX;

bool RenderEntity::LoadModelFromFile(const char* _pFilename)
{
	CommandList* pCmdListCpy = CommandList::Build(D3D12_COMMAND_LIST_TYPE_COPY, L"CopyContext");
	pCmdListCpy->Reset();

	m_pModel = nullptr;
	if (!AssimpLoader::LoadModel(DeviceD3D12::Instance(), pCmdListCpy, _pFilename, &m_pModel))
		return false;

	CommandQueue::Instance(D3D12_COMMAND_LIST_TYPE_COPY)->SubmitToQueue(pCmdListCpy);
	return true;
}

void RenderEntity::Update()
{

}