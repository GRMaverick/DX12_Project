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
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_pList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_pAllocator;
};
#endif // __CommandList_h__