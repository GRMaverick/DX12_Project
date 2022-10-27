#ifndef __CommandQueue_h__
#define __CommandQueue_h__

#include "Dx12Defines.h"

#include <d3d12.h>
#include <wrl.h>

#include "Interfaces/ICommandQueue.h"

#include "CommandListDx12.h"

namespace Artemis::Renderer::Device::Dx12
{
	class CommandQueueDx12 final : public Interfaces::ICommandQueue
	{
		friend class SwapChain;

	public:
		CommandQueueDx12();
		~CommandQueueDx12( void ) override;

		bool Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ECommandListType _type, const wchar_t* _pDebugName );

		void SubmitToQueue( Interfaces::ICommandList* _pList ) override { m_pAwaitingExecution.push_back( static_cast<CommandListDx12*>(_pList) ); } // Proper Synchronisation Required

		void ExecuteCommandLists( void ) override;

		void* GetDeviceObject() override { return m_pQueue.Get(); }

		UINT64 Signal() override;
		void   Wait() const override;
		void   Flush();

	private:
		D3D12_COMMAND_LIST_TYPE             m_eType;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
		UINT64                              m_uiFenceValue;
		HANDLE                              m_hFenceEvent;

		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pQueue;

		std::vector<CommandListDx12*> m_pAwaitingExecution;
	};
}

#endif // __CommandQueue_h__
