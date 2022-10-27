#ifndef __GpuResourceTable_h__
#define __GpuResourceTable_h__

namespace Artemis::Renderer::Interfaces
{
	class IShaderStage;
	class ISamplerState;
	class IGpuBufferResource;
}

namespace Artemis::Renderer::Device
{
	struct SamplerStateEntry
	{
		unsigned int Hash      = 0;
		unsigned int HeapIndex = 0;
	};

	class GpuResourceTable
	{
	public:
		GpuResourceTable( void );
		GpuResourceTable( Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS );
		~GpuResourceTable( void );

		void Initialise( Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS );
		void Destroy( void );

		bool IsInitialised( void ) const { return m_bIsInitialised; }

		bool SetTexture( const char* _pName, Interfaces::IGpuBufferResource* _pTexture ) const;
		bool SetConstantBuffer( const char* _pName, Interfaces::IGpuBufferResource* _pCBuffer ) const;
		bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _state ) const;

		Interfaces::IShaderStage* GetVShader( void ) const { return m_pVertexShader; }
		Interfaces::IShaderStage* GetPShader( void ) const { return m_pPixelShader; }

		unsigned long GetTextures( Interfaces::IGpuBufferResource*** _ppResources ) const;
		unsigned long GetConstantBuffers( Interfaces::IGpuBufferResource*** _ppResources ) const;
		unsigned long GetSamplers( Interfaces::ISamplerState*** _ppResources ) const;

		Interfaces::ISamplerState*      GetSampler( const char* _pName ) const;
		Interfaces::IGpuBufferResource* GetTexture( const char* _pName ) const;
		Interfaces::IGpuBufferResource* GetConstantBuffer( const char* _pName ) const;

	private:
		bool                      m_bIsInitialised = false;
		Interfaces::IShaderStage* m_pVertexShader  = nullptr;
		Interfaces::IShaderStage* m_pPixelShader   = nullptr;

		unsigned int m_uiNumberSamplers        = 0;
		unsigned int m_uiNumberTextures        = 0;
		unsigned int m_uiNumberConstantBuffers = 0;

		Interfaces::ISamplerState**      m_pSamplers        = nullptr;
		Interfaces::IGpuBufferResource** m_pTextures        = nullptr;
		Interfaces::IGpuBufferResource** m_pConstantBuffers = nullptr;
	};
}

#endif // __GpuResourceTable_h__
