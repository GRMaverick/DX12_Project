#ifndef __CommandList_h__
#define __CommandList_h__

#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

#include "DeviceD3D12.h"

#include "pix3.h"

//#define BREADCRUMB

class CommandList
{
	friend class ImGUIEngine;
	friend class CommandQueue;

public:
	CommandList(void);
	~CommandList(void);

	static CommandList* Build(D3D12_COMMAND_LIST_TYPE _type)
	{
		CommandList* pCommandList = nullptr;
		if (!DeviceD3D12::Instance()->CreateCommandList(D3D12_COMMAND_LIST_TYPE_COPY, &pCommandList, L"CPY"))
			return nullptr;
		pCommandList->Reset();
		return pCommandList;
	}

	bool Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName = L"");

	void Close(void);
	void Reset(void);

	void WriteBreadcrumb(UINT32 _breadcrumb);
	UINT32 ReadBreadcrumb(void);

	void ResourceBarrier(UINT32 _numBarriers, CD3DX12_RESOURCE_BARRIER* _pBarrier);
	void UpdateSubresource(ID3D12Resource* _pGPU, ID3D12Resource* _pCPU, UINT _intermediateOffset, UINT _firstSubresource, UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData);

	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], UINT _numRects, D3D12_RECT* _pRects);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_CLEAR_FLAGS _clearFlags, FLOAT _depth, UINT8 _stencil, UINT _numRects, D3D12_RECT* pRects);

	void SetPipelineState(ID3D12PipelineState* _pPipelineState);
	void SetRSViewports(UINT _numViewports, D3D12_VIEWPORT* _pViewport);
	void SetRSScissorRects(UINT _numRects, D3D12_RECT* _pScissorRects);
	void SetIAPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY _topology);
	void SetIAVertexBuffers(UINT StartSlot, UINT NumViews, D3D12_VERTEX_BUFFER_VIEW* pViews);
	void SetIAIndexBuffer(D3D12_INDEX_BUFFER_VIEW* pView);
	void SetOMRenderTargets(UINT _numRTs, D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, BOOL _bSingleHandleToDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle);

	void SetDescriptorHeaps(ID3D12DescriptorHeap* const* _pHeaps, UINT _numHeaps);
	void SetGraphicsRootSignature(ID3D12RootSignature* _pRootSignature);
	void SetGraphicsRootDescriptorTable(UINT _rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle);
	void SetGraphicsRoot32BitConstants(UINT _rootParameterIndex, UINT _num32BitValuesToSet, const void* _pSrcData, UINT _destOffsetIn32BitValues);
	void SetGraphicsRootConstantBufferView(UINT _rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS _gpuAddress);

	void DrawImGUI();
	void DrawIndexedInstanced(UINT _indicesPerInstance, UINT _instanceCount, UINT _startIndexLocation, UINT _baseVertexLocation, UINT _startInstanceLocation);

	template<typename... Args>
	void StartMarker(const char* _pFormatString, Args... _args)
	{
		PIXBeginEvent(m_pList.Get(), PIX_COLOR_INDEX(0), _pFormatString, _args...);
	}
	void EndMarker(void)
	{
		PIXEndEvent(m_pList.Get());
	}

private:
	D3D12_COMMAND_LIST_TYPE								m_Type;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pAllocator;

#if defined(_DEBUG) && defined(BREADCRUMB)
	UINT32*												m_BreadcrumbReadback;
	Microsoft::WRL::ComPtr<ID3D12Resource>				m_BreadcrumbBuffer;
#endif
};
#endif // __CommandList_h__