#ifndef __GpuResourceTable_h__
#define __GpuResourceTable_h__

namespace SysRenderer
{
	namespace Interfaces
	{
		class IShaderStage;
		class ISamplerState;
		class IGpuBufferResource;
	}

	namespace D3D12
	{
		struct SamplerStateEntry
		{
			unsigned int Hash = 0;
			unsigned int HeapIndex = 0;
		};

		class GpuResourceTable
		{
		public:
			GpuResourceTable(void);
			GpuResourceTable(Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS);
			~GpuResourceTable(void);

			void Initialise(Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS);
			void Destroy(void);

			bool IsInitialised(void) { return m_bIsInitialised; }

			bool SetTexture(const char* _pName, Interfaces::IGpuBufferResource* _pTexture);
			bool SetConstantBuffer(const char* _pName, Interfaces::IGpuBufferResource* _pCBuffer);
			bool SetSamplerState(const char* _pName, Interfaces::ISamplerState* _state);

			Interfaces::IShaderStage* GetVShader(void) { return m_pVertexShader; }
			Interfaces::IShaderStage* GetPShader(void) { return m_pPixelShader; }

			unsigned long GetTextures(Interfaces::IGpuBufferResource*** _ppResources);
			unsigned long GetConstantBuffers(Interfaces::IGpuBufferResource*** _ppResources);
			unsigned long GetSamplers(Interfaces::ISamplerState*** _ppResources);

		private:
			bool m_bIsInitialised = false;
			Interfaces::IShaderStage* m_pVertexShader = nullptr;
			Interfaces::IShaderStage* m_pPixelShader = nullptr;

			unsigned int m_NumberSamplers = 0;
			unsigned int m_NumberTextures = 0;
			unsigned int m_NumberConstantBuffers = 0;

			Interfaces::ISamplerState** m_pSamplers = nullptr;
			Interfaces::IGpuBufferResource** m_pTextures = nullptr;
			Interfaces::IGpuBufferResource** m_pConstantBuffers = nullptr;
		};
	}
}

#endif // __GpuResourceTable_h__