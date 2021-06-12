#ifndef __GpuResourceTable_h__
#define __GpuResourceTable_h__

class IShaderStage;
class IGpuBufferResource;

struct SamplerStateEntry
{
	unsigned int Hash = 0;
	unsigned int HeapIndex = 0;
};

class GpuResourceTable
{
public:
	GpuResourceTable(void);
	GpuResourceTable(IShaderStage* _pVS, IShaderStage* _pPS);
	~GpuResourceTable(void);

	void Initialise(IShaderStage* _pVS, IShaderStage* _pPS);
	void Destroy(void);

	bool IsInitialised(void) { return m_bIsInitialised; }

	bool SetTexture(const char* _pName, IGpuBufferResource* _pTexture);
	bool SetConstantBuffer(const char* _pName, IGpuBufferResource* _pCBuffer);
	bool SetSamplerState(const char* _pName, SamplerStateEntry _state);

	IShaderStage* GetVShader(void) { return m_pVertexShader; }
	IShaderStage* GetPShader(void) { return m_pPixelShader; }

	unsigned long GetTextures(IGpuBufferResource*** _ppResources);
	unsigned long GetConstantBuffers(IGpuBufferResource*** _ppResources);
	unsigned long GetSamplers(SamplerStateEntry** _ppResources);

private:
	bool m_bIsInitialised = false;
	IShaderStage* m_pVertexShader = nullptr;
	IShaderStage* m_pPixelShader = nullptr;

	unsigned int m_NumberSamplers = 0;
	unsigned int m_NumberTextures = 0;
	unsigned int m_NumberConstantBuffers = 0;

	SamplerStateEntry* m_pSamplers = nullptr;
	IGpuBufferResource** m_pTextures = nullptr;
	IGpuBufferResource** m_pConstantBuffers = nullptr;
};

#endif // __GpuResourceTable_h__