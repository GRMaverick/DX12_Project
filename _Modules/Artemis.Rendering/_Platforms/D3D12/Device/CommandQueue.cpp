#include "Defines.h"

#include "CommandQueue.h"

#include <assert.h>

#include "Artemis.Utilities\_Profiling\PixScopedEvent.h"

using namespace Microsoft::WRL;

extern const char* g_TypeToString[];

using namespace SysUtilities;

namespace SysRenderer
{
	namespace D3D12
	{
		CommandQueue::CommandQueue( void ):
			m_eType(),
			m_hFenceEvent( nullptr )
		{
			m_uiFenceValue = 0;

			m_pFence = nullptr;
			m_pQueue = nullptr;
		}

		CommandQueue::~CommandQueue( void )
		{
			if ( m_pFence )
				m_pFence.Reset();
			if ( m_pQueue )
				m_pQueue.Reset();
		}

		bool CommandQueue::Initialise( const Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, const D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName )
		{
			HRESULT hr = S_OK;

			D3D12_COMMAND_QUEUE_DESC desc = {};
			ZeroMemory( &desc, sizeof(D3D12_COMMAND_QUEUE_DESC) );
			desc.Type     = _type;
			desc.NodeMask = 0;
			desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

			hr = _pDevice->CreateCommandQueue( &desc, IID_PPV_ARGS( m_pQueue.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Command Queue Creation Failed" );
				return false;
			}

			hr = _pDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_pFence.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Fence Creation Failed" );
				return false;
			}

			m_hFenceEvent = CreateEvent( nullptr, FALSE, FALSE, NULL );
			if ( !m_hFenceEvent )
			{
				assert( false && "Fence Event Creation Failed" );
				return false;
			}

			m_pFence->SetName( _pDebugName );
			m_pQueue->SetName( _pDebugName );

			return true;
		}

		void CommandQueue::Flush()
		{
			PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Flush" );
			Signal();
			Wait();
		}

		UINT64 CommandQueue::Signal()
		{
			PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Signal" );
			m_pQueue->Signal( m_pFence.Get(), ++m_uiFenceValue );
			return m_uiFenceValue;
		}

		void CommandQueue::Wait() const
		{
			PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Wait" );
			if ( m_pFence->GetCompletedValue() < m_uiFenceValue )
			{
				m_pFence->SetEventOnCompletion( m_uiFenceValue, m_hFenceEvent );
				::WaitForSingleObject( m_hFenceEvent, static_cast<DWORD>(INFINITE) );
			}
		}

		void CommandQueue::ExecuteCommandLists( void )
		{
			PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "ExecuteCommandLists" );

			std::vector<ID3D12CommandList*> vpLists;
			for ( UINT listIdx = 0; listIdx < m_pAwaitingExecution.size(); ++listIdx )
			{
				m_pAwaitingExecution[listIdx]->Close();
				vpLists.push_back( m_pAwaitingExecution[listIdx]->m_pList.Get() );
			}

			if ( !vpLists.size() )
			{
				LogWarning( "Execute requested on empty queue" );
				return;
			}

			m_pQueue->ExecuteCommandLists( static_cast<UINT>(vpLists.size()), &vpLists[0] );

			Flush();

			if ( !m_pAwaitingExecution.size() )
			{
				LogWarning( "Execute requested on empty queue" );
				return;
			}

			for ( unsigned int listIdx = 0; listIdx < m_pAwaitingExecution.size(); ++listIdx )
			{
				delete m_pAwaitingExecution[listIdx];
				m_pAwaitingExecution.erase( m_pAwaitingExecution.begin() + listIdx );
			}
		}
	}
}
