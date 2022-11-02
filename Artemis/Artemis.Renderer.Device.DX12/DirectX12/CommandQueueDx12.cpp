#include "Dx12Defines.h"

#include "CommandQueueDx12.h"

#include <assert.h>

#include "Helpers\PixScopedEvent.h"

using namespace Microsoft::WRL;

extern const char* g_TypeToString[];

using namespace Artemis::Utilities;

namespace Artemis::Renderer::Device::Dx12
{
	CommandQueueDx12::CommandQueueDx12( void ) :
		m_eType(),
		m_hFenceEvent( nullptr )
	{
		m_uiFenceValue = 0;

		m_pFence = nullptr;
		m_pQueue = nullptr;
	}

	CommandQueueDx12::~CommandQueueDx12( void )
	{
		if ( m_pFence )
			m_pFence.Reset();
		if ( m_pQueue )
			m_pQueue.Reset();
	}

	bool CommandQueueDx12::Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ECommandListType _type, const wchar_t* _pDebugName )
	{
		HRESULT hr = S_OK;

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		D3D12_COMMAND_QUEUE_DESC desc = {};
		ZeroMemory( &desc, sizeof(D3D12_COMMAND_QUEUE_DESC) );
		desc.Type     = static_cast<D3D12_COMMAND_LIST_TYPE>(_type);
		desc.NodeMask = 0;
		desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

		hr = pDevice->CreateCommandQueue( &desc, IID_PPV_ARGS( m_pQueue.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			assert( false && "Command Queue Creation Failed" );
			return false;
		}

		hr = pDevice->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( m_pFence.GetAddressOf() ) );
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

	void CommandQueueDx12::Flush()
	{
		Helpers::PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Flush" );
		Signal();
		Wait();
	}

	unsigned long CommandQueueDx12::Signal()
	{
		Helpers::PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Signal" );
		m_pQueue->Signal( m_pFence.Get(), ++m_uiFenceValue );
		return m_uiFenceValue;
	}

	void CommandQueueDx12::Wait() const
	{
		Helpers::PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "Wait" );
		if ( m_pFence->GetCompletedValue() < m_uiFenceValue )
		{
			m_pFence->SetEventOnCompletion( m_uiFenceValue, m_hFenceEvent );
			::WaitForSingleObject( m_hFenceEvent, static_cast<DWORD>(INFINITE) );
		}
	}

	void CommandQueueDx12::ExecuteCommandLists( void )
	{
		Helpers::PixScopedEvent rEvent( m_pQueue.Get(), "%s: %s", g_TypeToString[m_eType], "ExecuteCommandLists" );

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
