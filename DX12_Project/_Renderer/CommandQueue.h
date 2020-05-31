#ifndef __CommandQueue_h__
#define __CommandQueue_h__

#include <d3d12.h>
#include <wrl.h>

#include "CommandList.h"

class CommandQueue
{
	friend class SwapChain;

public:
	CommandQueue(void);
	~CommandQueue(void);

	bool	Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type);

	void	ExecuteCommandLists(CommandList* _pList, UINT _numLists);

	UINT64	Signal();
	void	Wait();
	void	Flush();

private:
	Microsoft::WRL::ComPtr<ID3D12Fence>			m_pFence;
	UINT64										m_FenceValue;
	HANDLE										m_FenceEvent;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue>	m_pQueue;
};

#endif // __CommandQueue_h__

