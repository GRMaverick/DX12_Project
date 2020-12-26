#include "Defines.h"

#include "CommandQueue.h"

#include <assert.h>

#include "PixScopedEvent.h"

using namespace Microsoft::WRL;

extern const char* g_TypeToString[];

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

bool CommandQueue::Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName)
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

	m_pFence->SetName(_pDebugName);
	m_pQueue->SetName(_pDebugName);

	return true;
}

void CommandQueue::Flush()
{
	PixScopedEvent rEvent(m_pQueue.Get(), "%s: %s", g_TypeToString[m_Type], "Flush");
	Signal();
	Wait();
}

UINT64 CommandQueue::Signal()
{
	PixScopedEvent rEvent(m_pQueue.Get(), "%s: %s", g_TypeToString[m_Type], "Signal");
	m_pQueue->Signal(m_pFence.Get(), ++m_FenceValue);
	return m_FenceValue;
}
void CommandQueue::Wait()
{
	PixScopedEvent rEvent(m_pQueue.Get(), "%s: %s", g_TypeToString[m_Type], "Wait");
	if (m_pFence->GetCompletedValue() < m_FenceValue)
	{
		m_pFence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(INFINITE));
	}
}

void CommandQueue::ExecuteCommandLists(void)
{
	PixScopedEvent rEvent(m_pQueue.Get(), "%s: %s", g_TypeToString[m_Type], "ExecuteCommandLists");

	std::vector<ID3D12CommandList*> vpLists;
	for (UINT listIdx = 0; listIdx < m_pAwaitingExecution.size(); ++listIdx)
	{
		m_pAwaitingExecution[listIdx]->Close();
		vpLists.push_back(m_pAwaitingExecution[listIdx]->m_pList.Get());
	}

	if (!vpLists.size())
	{
		LogWarning_Renderer("Execute requested on empty queue");
		return;
	}

	m_pQueue->ExecuteCommandLists((UINT)vpLists.size(), &vpLists[0]);
	
	Flush();

	for (INT listIdx = m_pAwaitingExecution.size() - 1; listIdx >= 0 ; --listIdx)
	{
		delete m_pAwaitingExecution[listIdx];
		m_pAwaitingExecution.pop_back();
	}
}