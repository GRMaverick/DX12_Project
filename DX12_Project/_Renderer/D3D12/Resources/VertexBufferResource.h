#ifndef __VertexBufferResource_h__
#define __VertexBufferResource_h__

#include "IBufferResource.h"

class VertexBufferResource : public IBufferResource
{
public:
	VertexBufferResource(void) { }
	~VertexBufferResource(void) { }

	void SetView(D3D12_VERTEX_BUFFER_VIEW _vbv) { m_View = _vbv; }
	D3D12_VERTEX_BUFFER_VIEW GetView() { return m_View; }

private:
	D3D12_VERTEX_BUFFER_VIEW m_View;
};

#endif // __VertexBufferResource_h__
