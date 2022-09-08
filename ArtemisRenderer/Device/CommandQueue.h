#ifndef __CommandQueue_h__
#define __CommandQueue_h__

//#include "Defines.h"

#include <d3d12.h>
#include <vector>

namespace ArtemisRenderer::Device
{
	class CommandList;

	class CommandQueue
	{
		friend class SwapChain;

	public:
		static CommandQueue* Instance(D3D12_COMMAND_LIST_TYPE _type);

		CommandQueue(void);
		~CommandQueue(void);

		bool	Initialise(ID3D12Device* _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName);

		void	SubmitToQueue(CommandList* _pList) { m_pAwaitingExecution.push_back(_pList); }	// Proper Synchronisation Required

		void	ExecuteCommandLists(void);

		UINT64	Signal();
		void	Wait();
		void	Flush();

	private:

		D3D12_COMMAND_LIST_TYPE						m_Type;
		ID3D12Fence*								m_pFence;
		UINT64										m_FenceValue;
		HANDLE										m_FenceEvent;

		ID3D12CommandQueue*							m_pQueue;
		std::vector<CommandList*>					m_pAwaitingExecution;
	};
}

#endif // __CommandQueue_h__