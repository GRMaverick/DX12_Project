#ifndef __Texture2DResource_h__
#define __Texture2DResource_h__

#include <d3d12.h>
#include <wrl.h>

#include "IBufferResource.h"

class CommandList;
class DescriptorHeap;

class Texture2DResource : public IBufferResource
{
public:
	Texture2DResource(
		const wchar_t* _pWstrFilename, 
		const bool _bIsDDS, 
		DescriptorHeap* _pTargetSRVHeap, 
		ID3D12Device* _pDevice, 
		CommandList* _pCmdList,
		const char* _pDebugName = nullptr
	);

	~Texture2DResource(void);

	UINT32 GetHeapIndex(void) { return m_HeapIndex; }
	
private:
	UINT m_HeapIndex = 0;

	bool CreateFromDDS(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList);
	bool CreateFromWIC(const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandList* _pCmdList);
};

#endif __Texture2DResource_h__