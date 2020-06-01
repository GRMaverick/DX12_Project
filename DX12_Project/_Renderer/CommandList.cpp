#include "Defines.h"

#include "CommandList.h"

#include <assert.h>

using namespace Microsoft::WRL;

CommandList::CommandList(void)
{
	m_pList = nullptr;
	m_pAllocator = nullptr;
}

CommandList::~CommandList(void)
{
	if (m_pList) m_pList.Reset();
	if (m_pAllocator) m_pAllocator.Reset();
}

bool CommandList::Initialise(ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type)
{
	HRESULT hr = S_OK;

	// Command Allocator
	{
		hr = _pDevice->CreateCommandAllocator(_type, IID_PPV_ARGS(m_pAllocator.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "Command Allocator Creation Failed");
			return false;
		}
	}

	// Command List
	{
		hr = _pDevice->CreateCommandList(0, _type, m_pAllocator.Get(), nullptr, IID_PPV_ARGS(m_pList.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "Command List Creation Failed");
			return false;
		}
	}

	Close();
	
	return true;
}

void CommandList::Close(void)
{
	m_pList->Close();
}

void CommandList::Reset(void)
{
	m_pAllocator->Reset();
	m_pList->Reset(m_pAllocator.Get(), nullptr);
}

void CommandList::ResourceBarrier(UINT32 _numBarriers, CD3DX12_RESOURCE_BARRIER* _pBarrier)
{
	m_pList->ResourceBarrier(_numBarriers, _pBarrier);
}
void CommandList::UpdateSubresource(ID3D12Resource* _pGPU, ID3D12Resource* _pCPU, UINT _intermediateOffset, UINT _firstSubresource, UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData)
{
	UpdateSubresources(m_pList.Get(), _pGPU, _pCPU, _intermediateOffset, _firstSubresource, _numSubresources, _pSubresourceData);
}
void CommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], UINT _numRects, D3D12_RECT* _pRects)
{
	m_pList->ClearRenderTargetView(_cpuHandle, _pColor, _numRects, _pRects);
}
void CommandList::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_CLEAR_FLAGS _clearFlags, FLOAT _depth, UINT8 _stencil, UINT _numRects, D3D12_RECT* _pRects)
{
	m_pList->ClearDepthStencilView(_cpuHandle, _clearFlags, _depth, _stencil, _numRects, _pRects);
}