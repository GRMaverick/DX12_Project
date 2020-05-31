#ifndef __IndexBufferResource_h__
#define __IndexBufferResource_h__

#include "IBufferResource.h"

class IndexBufferResource : public IBufferResource
{
public:
	IndexBufferResource(void) { }
	~IndexBufferResource(void) { }

	void SetView(D3D12_INDEX_BUFFER_VIEW _ibv) { m_View = _ibv; }
	D3D12_INDEX_BUFFER_VIEW GetView() { return m_View; }

private:
	D3D12_INDEX_BUFFER_VIEW m_View;
};

#endif // __IndexBufferResource_h__