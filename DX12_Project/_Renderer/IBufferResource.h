#ifndef __BufferResource_h__
#define __BufferResource_h__

#include <d3d12.h>
#include <wrl.h>

class IBufferResource
{
public:
	~IBufferResource(void)
	{
		if (m_CPUBuffer) m_CPUBuffer.Reset();
		if (m_GPUBuffer) m_GPUBuffer.Reset();
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> GetCPUBuffer(void) { return m_CPUBuffer; }
	Microsoft::WRL::ComPtr<ID3D12Resource> GetGPUBuffer(void) { return m_GPUBuffer; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> m_CPUBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_GPUBuffer;
};

#endif // __BufferResource_h__