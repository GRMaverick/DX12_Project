#ifndef __GpuResourceTable_h__
#define __GpuResourceTable_h__

class IShader;
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
	GpuResourceTable(IShader* _pVS, IShader* _pPS);
	~GpuResourceTable(void);

	void Initialise(IShader* _pVS, IShader* _pPS);
	void Destroy(void);

	bool IsInitialised(void) { return m_bIsInitialised; }

	bool SetTexture(const char* _pName, IGpuBufferResource* _pTexture);
	bool SetConstantBuffer(const char* _pName, IGpuBufferResource* _pCBuffer);
	bool SetSamplerState(const char* _pName, SamplerStateEntry _state);

	IShader* GetVShader(void) { return m_pVertexShader; }
	IShader* GetPShader(void) { return m_pPixelShader; }

	unsigned long GetTextures(IGpuBufferResource*** _ppResources);
	unsigned long GetConstantBuffers(IGpuBufferResource*** _ppResources);
	unsigned long GetSamplers(SamplerStateEntry** _ppResources);

private:
	bool m_bIsInitialised = false;
	IShader* m_pVertexShader = nullptr;
	IShader* m_pPixelShader = nullptr;

	SamplerStateEntry* m_pSamplers = nullptr;
	IGpuBufferResource** m_pTextures = nullptr;
	IGpuBufferResource** m_pConstantBuffers = nullptr;
};

#endif // __GpuResourceTable_h__