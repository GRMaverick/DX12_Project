#include "Defines.h"

#include "CommandQueue.h"

#include <assert.h>

using namespace Microsoft::WRL;

CommandQueue::CommandQueue(void)
{
	m_FenceValue = 0;

	m_pFence = nullptr;
	m_pQueue = nullptr;
}
CommandQueue::~CommandQueue(void)
{
	if (m_pFence) m_pFence.Reset();
	if (m_pQueue) m_pQueue.Reset();
}

bool CommandQueue::Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type)
{
	HRESULT hr = S_OK;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	ZeroMemory(&desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	desc.Type = _type;
	desc.NodeMask = 0;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	hr = _pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_pQueue.GetAddressOf()));
	if (FAILED(hr))
	{
		assert(false && "Command Queue Creation Failed");
		return false;
	}

	hr = _pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_pFence.GetAddressOf()));
	if (FAILED(hr))
	{
		assert(false && "Fence Creation Failed");
		return false;
	}

	m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_FenceEvent)
	{
		assert(false && "Fence Event Creation Failed");
		return false;
	}

	return true;
}

void CommandQueue::Flush()
{
	Signal();
	Wait();
}

UINT64 CommandQueue::Signal()
{
	m_pQueue->Signal(m_pFence.Get(), ++m_FenceValue);
	return m_FenceValue;
}
void CommandQueue::Wait()
{
	if (m_pFence->GetCompletedValue() < m_FenceValue)
	{
		m_pFence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(INFINITE));
	}
}

void CommandQueue::ExecuteCommandLists(CommandList* _pLists, UINT _numLists)
{
	std::vector<ID3D12CommandList*> vpLists;
	for (UINT listIdx = 0; listIdx < _numLists; ++listIdx)
	{
		_pLists[listIdx].Close();
		vpLists.push_back(_pLists[listIdx].m_pList.Get());
	}

	m_pQueue->ExecuteCommandLists(vpLists.size(), &vpLists[0]);
}