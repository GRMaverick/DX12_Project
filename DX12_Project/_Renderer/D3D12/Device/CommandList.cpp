#include "Defines.h"

#include "CommandList.h"

#include <assert.h>

#include "PixScopedEvent.h"

using namespace Microsoft::WRL;

const char* g_TypeToString[]
{
	"GFX List",
	"Bundle List",
	"Compute List",
	"Copy List",
	"Video Decode",
	"Video Process",
	"Video Encode",
};

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

bool CommandList::Initialise(ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName)
{
	HRESULT hr = S_OK;
	m_Type = _type;

	// Command Allocator
	{
		hr = _pDevice->CreateCommandAllocator(_type, IID_PPV_ARGS(m_pAllocator.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "Command Allocator Creation Failed");
			return false;
		}
		m_pAllocator->SetName(_pDebugName);
	}

	// Command List
	{
		hr = _pDevice->CreateCommandList(0, _type, m_pAllocator.Get(), nullptr, IID_PPV_ARGS(m_pList.GetAddressOf()));
		if (FAILED(hr))
		{
			assert(false && "Command List Creation Failed");
			return false;
		}
		m_pList->SetName(_pDebugName);
	}


#if defined(_DEBUG) && defined(BREADCRUMB)
	// Init Breadcrumb Debugging
	{
		VALIDATE_D3D(_pDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT32)),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_BreadcrumbBuffer)));

		m_BreadcrumbBuffer->Map(0, nullptr, (void**)&m_BreadcrumbReadback);
	}
#endif

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

void CommandList::WriteBreadcrumb(UINT32 _breadcrumb)
{
#if defined(_DEBUG) && defined(BREADCRUMB)
	ComPtr<ID3D12GraphicsCommandList2> pList2 = nullptr;
	VALIDATE_D3D(m_pList.As(&pList2));

	D3D12_WRITEBUFFERIMMEDIATE_MODE modes = D3D12_WRITEBUFFERIMMEDIATE_MODE_DEFAULT;
	D3D12_WRITEBUFFERIMMEDIATE_PARAMETER params;
	params.Dest = m_BreadcrumbBuffer->GetGPUVirtualAddress();
	params.Value = _breadcrumb;
	pList2->WriteBufferImmediate(1, &params, &modes);
#endif
}
UINT32 CommandList::ReadBreadcrumb(void)
{
#if defined(_DEBUG) && defined(BREADCRUMB)
	return *m_BreadcrumbReadback;
#else
	return 0;
#endif
}
void CommandList::ResourceBarrier(UINT32 _numBarriers, CD3DX12_RESOURCE_BARRIER* _pBarrier)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "ResourceBarrier");
	m_pList->ResourceBarrier(_numBarriers, _pBarrier);
}

void CommandList::UpdateSubresource(ID3D12Resource* _pGPU, ID3D12Resource* _pCPU, UINT _intermediateOffset, UINT _firstSubresource, UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "UpdateSubresource");
	UpdateSubresources(m_pList.Get(), _pGPU, _pCPU, _intermediateOffset, _firstSubresource, _numSubresources, _pSubresourceData);
}

void CommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], UINT _numRects, D3D12_RECT* _pRects)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "ClearRenderTargetView");
	m_pList->ClearRenderTargetView(_cpuHandle, _pColor, _numRects, _pRects);
}

void CommandList::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_CLEAR_FLAGS _clearFlags, FLOAT _depth, UINT8 _stencil, UINT _numRects, D3D12_RECT* _pRects)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "ClearDepthStencilView");
	m_pList->ClearDepthStencilView(_cpuHandle, _clearFlags, _depth, _stencil, _numRects, _pRects);
}

void CommandList::SetPipelineState(ID3D12PipelineState* _pPipelineState)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetPipelineState");
	m_pList->SetPipelineState(_pPipelineState);
}

void CommandList::SetGraphicsRootSignature(ID3D12RootSignature* _pRootSignature)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetGraphicsRootSignature");
	m_pList->SetGraphicsRootSignature(_pRootSignature);
}

void CommandList::SetGraphicsRoot32BitConstants(UINT _rootParameterIndex, UINT _num32BitValuesToSet, const void* _pSrcData, UINT _destOffsetIn32BitValues)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetGraphicsRoot32BitConstants");
	m_pList->SetGraphicsRoot32BitConstants(_rootParameterIndex, _num32BitValuesToSet, _pSrcData, _destOffsetIn32BitValues);
}

void CommandList::SetRSViewports(UINT _numViewports, D3D12_VIEWPORT* _pViewport)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetRSViewports");
	m_pList->RSSetViewports(_numViewports, _pViewport);
}

void CommandList::SetRSScissorRects(UINT _numRects, D3D12_RECT* _pScissorRects)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetRSScissorRects");
	m_pList->RSSetScissorRects(_numRects, _pScissorRects);
}

void CommandList::SetIAPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY _topology)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetIAPrimitiveTopology");
	m_pList->IASetPrimitiveTopology(_topology);
}

void CommandList::SetIAVertexBuffers(UINT _startSlot, UINT _numViews, D3D12_VERTEX_BUFFER_VIEW* _pViews)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetIAVertexBuffers");
	m_pList->IASetVertexBuffers(_startSlot, _numViews, _pViews);
}

void CommandList::SetIAIndexBuffer(D3D12_INDEX_BUFFER_VIEW* _pView)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetIAIndexBuffer");
	m_pList->IASetIndexBuffer(_pView);
}

void CommandList::SetOMRenderTargets(UINT _numRTs, D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, BOOL _bSingleHandleToDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetOMRenderTargets");
	m_pList->OMSetRenderTargets(_numRTs, _rtCpuDescHandle, _bSingleHandleToDescriptor, _dsvCpuDescHandle);
}

void CommandList::DrawIndexedInstanced(UINT _indicesPerInstance, UINT _instanceCount, UINT _startIndexLocation, UINT _baseVertexLocation, UINT _startInstanceLocation)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "DrawIndexedInstanced");
	m_pList->DrawIndexedInstanced(_indicesPerInstance, _instanceCount, _startIndexLocation, _baseVertexLocation, _startInstanceLocation);
}

void CommandList::SetGraphicsRootDescriptorTable(UINT _rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetGraphicsRootDescriptorTable");
	m_pList->SetGraphicsRootDescriptorTable(_rootParameterIndex, _gpuHandle);
}
void CommandList::SetDescriptorHeaps(ID3D12DescriptorHeap* const* _pHeaps, UINT _numHeaps)
{
	PixScopedEvent rEvent(m_pList.Get(), "%s: %s", g_TypeToString[m_Type], "SetDescriptorHeaps");
	m_pList->SetDescriptorHeaps(_numHeaps, _pHeaps);
}