#ifndef __Texture2DResource_h__
#define __Texture2DResource_h__

#include <d3d12.h>
#include <wrl.h>

#include "IBufferResource.h"

class Texture2DResource : public IBufferResource
{
public:
	Texture2DResource(void);
	~Texture2DResource(void);

	bool Initialise(UINT32 _heapIndex);

	UINT32 GetHeapIndex(void) { return m_HeapIndex; }
	
private:
	UINT32 m_HeapIndex = 0;
};

#endif __Texture2DResource_h__