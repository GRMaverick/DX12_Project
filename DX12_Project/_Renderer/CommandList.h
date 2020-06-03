#ifndef __CommandList_h__
#define __CommandList_h__

#include <d3d12.h>
#include <wrl.h>
#include "d3dx12.h"

class CommandList
{
	friend class CommandQueue;

public:
	CommandList(void);
	~CommandList(void);

	bool Initialise(Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type);

	void Close(void);
	void Reset(void);

	void ResourceBarrier(UINT32 _numBarriers, CD3DX12_RESOURCE_BARRIER* _pBarrier);
	void UpdateSubresource(ID3D12Resource* _pGPU, ID3D12Resource* _pCPU, UINT _intermediateOffset, UINT _firstSubresource, UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData);

	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], UINT _numRects, D3D12_RECT* _pRects);
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, D3D12_CLEAR_FLAGS _clearFlags, FLOAT _depth, UINT8 _stencil, UINT _numRects, D3D12_RECT* pRects);

	void SetPipelineState(ID3D12PipelineState* _pPipelineState);
	void SetGraphicsRootSignature(ID3D12RootSignature* _pRootSignature);
	void SetRSViewports(UINT _numViewports, D3D12_VIEWPORT* _pViewport);
	void SetRSScissorRects(UINT _numRects, D3D12_RECT* _pScissorRects);
	void SetIAPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY _topology);
	void SetIAVertexBuffers(UINT StartSlot, UINT NumViews, D3D12_VERTEX_BUFFER_VIEW* pViews);
	void SetIAIndexBuffer(D3D12_INDEX_BUFFER_VIEW* pView);
	void SetOMRenderTargets(UINT _numRTs, D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, BOOL _bSingleHandleToDescriptor, D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle);
	void SetGraphicsRoot32BitConstants(UINT _rootParameterIndex, UINT _num32BitValuesToSet, const void* _pSrcData, UINT _destOffsetIn32BitValues);
	void DrawIndexedInstanced(UINT _indicesPerInstance, UINT _instanceCount, UINT _startIndexLocation, UINT _baseVertexLocation, UINT _startInstanceLocation);
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pAllocator;
};
#endif // __CommandList_h__