#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <wrl.h>

#include <d3d12.h>

#include "IRenderer.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "DeviceD3D12.h"
#include "DescriptorHeap.h"
#include "SwapChain.h"
#include "VertexBufferResource.h"
#include "IndexBufferResource.h"

class RendererD3D12 : public IRenderer
{
public:
	RendererD3D12(void);
	~RendererD3D12(void);

	virtual bool Initialise(CoreWindow* _pWindow) override final;
	virtual bool Render(void) override final;

private:
	DeviceD3D12			m_Device;
	SwapChain*			m_pSwapChain;

	CommandList*		m_pGFXCommandList;
	CommandList*		m_pCopyCommandList;

	CommandQueue*		m_pCopyCommandQueue;
	CommandQueue*		m_pGFXCommandQueue;

	VertexBufferResource*		m_pVertexBuffer;
	IndexBufferResource*		m_pIndexBuffer;

	bool LoadCube();
};

#endif // __RendererD3D12_h__
