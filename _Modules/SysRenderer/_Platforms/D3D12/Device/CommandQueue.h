#ifndef __CommandQueue_h__
#define __CommandQueue_h__

#include "Defines.h"

#include <d3d12.h>
#include <wrl.h>

#include "CommandList.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class CommandQueue
		{
			friend class SwapChain;

		public:
			static CommandQueue* Instance( D3D12_COMMAND_LIST_TYPE _type )
			{
				switch ( _type )
				{
					case D3D12_COMMAND_LIST_TYPE_DIRECT:
					{
						static CommandQueue* pGfx;
						if ( pGfx )
							return pGfx;

						pGfx = new CommandQueue();

						if ( !DeviceD3D12::Instance()->CreateCommandQueue( _type, &pGfx, L"GFX" ) )
							return nullptr;
						return pGfx;
					}
					case D3D12_COMMAND_LIST_TYPE_COPY:
					{
						static CommandQueue* pCopy;
						if ( pCopy )
							return pCopy;

						pCopy = new CommandQueue();

						if ( !DeviceD3D12::Instance()->CreateCommandQueue( _type, &pCopy, L"CPY" ) )
							return nullptr;
						return pCopy;
					}
					case D3D12_COMMAND_LIST_TYPE_COMPUTE:
					{
						static CommandQueue* pCompute;
						if ( pCompute )
							return pCompute;

						pCompute = new CommandQueue();

						if ( !DeviceD3D12::Instance()->CreateCommandQueue( _type, &pCompute, L"Compute" ) )
							return nullptr;
						return pCompute;
					}
					default:
						return nullptr; // Unsupported Command List type.
				}
			}

			~CommandQueue( void );

			bool Initialise( Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName );

			void SubmitToQueue( CommandList* _pList ) { m_pAwaitingExecution.push_back( _pList ); } // Proper Synchronisation Required

			void ExecuteCommandLists( void );

			UINT64 Signal();
			void   Wait() const;
			void   Flush();

		private:
			CommandQueue( void );

			D3D12_COMMAND_LIST_TYPE             m_eType;
			Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
			UINT64                              m_uiFenceValue;
			HANDLE                              m_hFenceEvent;

			Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pQueue;

			std::vector<CommandList*> m_pAwaitingExecution;
		};
	}
}

#endif // __CommandQueue_h__
