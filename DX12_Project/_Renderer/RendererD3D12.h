#ifndef __RendererD3D12_h__
#define __RendererD3D12_h__

#include <wrl.h>

#include <d3d12.h>

#include "IRenderer.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "DeviceD3D12.h"
#include "DescriptorHeap.h"
#include "IndexBufferResource.h"
#include "ShaderCache.h"
#include "SwapChain.h"
#include "VertexBufferResource.h"
#include "Texture2DResource.h"
#include "ShaderCache.h"

#include <DirectXMath.h>

struct Model;

class RendererD3D12 : public IRenderer
{
public:
	RendererD3D12(void);
	~RendererD3D12(void);

	virtual bool Initialise(CoreWindow* _pWindow) override final;
	virtual void Update(float _deltaTime) override final;
	virtual bool Render(void) override final;

private:
	DeviceD3D12										m_Device;
	SwapChain*										m_pSwapChain;

	CommandList*									m_pGFXCommandList;
	CommandList*									m_pCopyCommandList;

	CommandQueue*									m_pCopyCommandQueue;
	CommandQueue*									m_pGFXCommandQueue;

	Texture2DResource* m_randomResource;

	Model*											m_pModel;

	ShaderCache										m_ShaderCache;
	DescriptorHeap*									m_pDescHeapSRV;
	DescriptorHeap*									m_pDescHeapSampler;

	// Refactor Required
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pBasicRS = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature>		m_pAlbedoRS = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pBasicPSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>		m_pAlbedoPSO = nullptr;

	DirectX::XMMATRIX m_ModelMatrix;
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ProjectionMatrix;
	float m_AspectRatio;
	float m_FieldOfView;
	float m_CurrentRotation = 0.0f;

	bool LoadContent();
	bool CreatePipelineState();
};

#endif // __RendererD3D12_h__
