#include "DeviceState.h"

#include <map>

#include "ShaderDx12.h"
#include "SamplerStateDx12.h"
#include "GpuResourceDx12.h"

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Device::Dx12
{
	void DeviceState::Reinitialise( Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS )
	{
		if ( m_bIsInitialised )
			Destroy();

		m_pVertexShader = static_cast<ShaderDx12*>(_pVS);
		m_pPixelShader  = static_cast<ShaderDx12*>(_pPS);

		std::map<unsigned int, const char*> cbBinds;
		std::map<unsigned int, const char*> texBinds;
		std::map<unsigned int, const char*> sampBinds;

		const Interfaces::IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS->NumberBuffers; ++i )
		{
			cbBinds[cbParamsVS->Buffers[i].BindPoint] = cbParamsVS->Buffers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsVS->NumberSamplers; ++i )
		{
			sampBinds[cbParamsVS->Samplers[i].BindPoint] = cbParamsVS->Samplers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsVS->NumberTextures; ++i )
		{
			texBinds[cbParamsVS->Textures[i].BindPoint] = cbParamsVS->Textures[i].Name;
		}

		const Interfaces::IConstantBufferParameters* cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS->NumberBuffers; ++i )
		{
			if ( cbBinds.find( cbParamsPS->Buffers[i].BindPoint ) == cbBinds.end() )
				cbBinds[cbParamsPS->Buffers[i].BindPoint] = cbParamsPS->Buffers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsPS->NumberSamplers; ++i )
		{
			if ( sampBinds.find( cbParamsPS->Samplers[i].BindPoint ) == sampBinds.end() )
				sampBinds[cbParamsPS->Samplers[i].BindPoint] = cbParamsPS->Samplers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsPS->NumberTextures; ++i )
		{
			if ( texBinds.find( cbParamsPS->Textures[i].BindPoint ) == texBinds.end() )
				texBinds[cbParamsPS->Textures[i].BindPoint] = cbParamsPS->Textures[i].Name;
		}

		m_uiNumberSamplers        = static_cast<unsigned int>(sampBinds.size());
		m_uiNumberTextures        = static_cast<unsigned int>(texBinds.size());
		m_uiNumberConstantBuffers = static_cast<unsigned int>(cbBinds.size());

		m_pSamplers        = new SamplerStateDx12*[m_uiNumberSamplers];
		m_pTextures        = new GpuResourceDx12*[m_uiNumberTextures];
		m_pConstantBuffers = new GpuResourceDx12*[m_uiNumberConstantBuffers];

		m_stats.ShaderUpdates++;
		SetDirty( kDirtyShaders | kDirtyPipelineState );

		m_bIsInitialised = true;
	}

	void DeviceState::Destroy( void )
	{
		if ( m_bIsInitialised )
		{
			const Interfaces::IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
			for ( unsigned int i = 0; i < cbParamsVS->NumberBuffers; ++i )
			{
				m_pConstantBuffers[i] = nullptr;
			}
			delete[] m_pConstantBuffers;
			m_pConstantBuffers = nullptr;
			m_pVertexShader    = nullptr;

			//ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
			delete[] m_pSamplers;

			for ( unsigned int i = 0; i < cbParamsVS->NumberTextures; ++i )
			{
				m_pTextures[i] = nullptr;
			}
			delete[] m_pTextures;
			m_pTextures = nullptr;

			m_pPixelShader = nullptr;
		}
		m_bIsInitialised = false;
	}

	bool DeviceState::SetTexture( const char* _pName, Interfaces::IGpuResource* _pTexture )
	{
		const Interfaces::IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsVS->Textures[i].Name, _pName, ARRAYSIZE( cbParamsVS->Textures[i].Name ) ) == 0 )
			{
				m_pTextures[cbParamsVS->Textures[i].BindPoint] = static_cast<GpuResourceDx12*>(_pTexture);

				m_stats.TextureUpdates++;
				SetDirty( kDirtyTexture );

				return true;
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsPS->Textures[i].Name, _pName, ARRAYSIZE( cbParamsPS->Textures[i].Name ) ) == 0 )
			{
				m_pTextures[cbParamsPS->Textures[i].BindPoint] = static_cast<GpuResourceDx12*>(_pTexture);

				m_stats.TextureUpdates++;
				SetDirty( kDirtyTexture );

				return true;
			}
		}

		// Can ignore this CB Binding, Shaders aren't expecting it
		// assert(false && "SRV Resource Binding Failed");

		return false;
	}

	bool DeviceState::SetConstantBuffer( const char* _pName, Interfaces::IGpuResource* _pConstantBuffer )
	{
		const Interfaces::IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsVS->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsVS->Buffers[i].Name ) ) == 0 )
			{
				m_pConstantBuffers[cbParamsVS->Buffers[i].BindPoint] = static_cast<GpuResourceDx12*>(_pConstantBuffer);

				m_stats.ConstantBufferUpdates++;
				SetDirty( kDirtyConstantBuffer );

				return true;
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsPS->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsPS->Buffers[i].Name ) ) == 0 )
			{
				m_pConstantBuffers[cbParamsPS->Buffers[i].BindPoint] = static_cast<GpuResourceDx12*>(_pConstantBuffer);

				m_stats.ConstantBufferUpdates++;
				SetDirty( kDirtyConstantBuffer );

				return true;
			}
		}

		// Can ignore this CB Binding, Shaders aren't expecting it
		// assert(false && "SRV Resource Binding Failed");

		return false;
	}

	bool DeviceState::SetSampler( const char* _pName, Interfaces::ISamplerState* _pSampler )
	{
		const Interfaces::IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsVs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsVs->Samplers[i].Name ) ) == 0 )
			{
				m_pSamplers[cbParamsVs->Samplers[i].BindPoint] = static_cast<SamplerStateDx12*>(_pSampler);

				m_stats.SamplerStateUpdates++;
				SetDirty( kDirtySamplerState );

				return true;
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsPs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsPs->Samplers[i].Name ) ) == 0 )
			{
				m_pSamplers[cbParamsPs->Samplers[i].BindPoint] = static_cast<SamplerStateDx12*>(_pSampler);

				m_stats.SamplerStateUpdates++;
				SetDirty( kDirtySamplerState );

				return true;
			}
		}

		return false;
	}

	bool DeviceState::SetBlendState( const Interfaces::BlendDesc& _desc )
	{
		assert( sizeof(D3D12_BLEND_DESC) == sizeof(Interfaces::BlendDesc) );
		if ( memcmp( &BlendState, &_desc, sizeof( D3D12_BLEND_DESC ) ) != 0 )
		{
			memcpy( &BlendState, &_desc, sizeof( D3D12_BLEND_DESC ) );
			SetDirty( kDirtyPipelineState );
		}
		return true;
	}

	bool DeviceState::SetDepthStencilState( const Interfaces::DepthStencilDesc& _desc )
	{
		assert( sizeof(D3D12_DEPTH_STENCIL_DESC) == sizeof(Interfaces::DepthStencilDesc) );
		if ( memcmp( &DepthStencilState, &_desc, sizeof( D3D12_DEPTH_STENCIL_DESC ) ) != 0 )
		{
			memcpy( &DepthStencilState, &_desc, sizeof( D3D12_DEPTH_STENCIL_DESC ) );
			SetDirty( kDirtyPipelineState );
		}
		return true;
	}

	bool DeviceState::SetRasterizerState( const Interfaces::RasteriserStateDesc& _desc )
	{
		assert( sizeof(D3D12_RASTERIZER_DESC) == sizeof(Interfaces::RasteriserStateDesc) );
		if ( memcmp( &RasterizerState, &_desc, sizeof( D3D12_RASTERIZER_DESC ) ) != 0 )
		{
			memcpy( &RasterizerState, &_desc, sizeof( D3D12_RASTERIZER_DESC ) );
			SetDirty( kDirtyPipelineState );
		}
		return true;
	}

	void DeviceState::SetDirty( const unsigned int _dirtyFlag )
	{
		DirtyFlags |= _dirtyFlag;
	}

	bool DeviceState::IsDirty( const unsigned int _dirtyFlags ) const
	{
		return (DirtyFlags & _dirtyFlags) == _dirtyFlags;
	}

	unsigned long DeviceState::GetTextures( GpuResourceDx12*** _pppResources ) const
	{
		(*_pppResources) = m_pTextures;
		return m_uiNumberTextures;
	}

	unsigned long DeviceState::GetConstantBuffers( GpuResourceDx12*** _pppResources ) const
	{
		(*_pppResources) = m_pConstantBuffers;
		return m_uiNumberConstantBuffers;
	}

	unsigned long DeviceState::GetSamplers( SamplerStateDx12*** _ppResources ) const
	{
		(*_ppResources) = m_pSamplers;
		return m_uiNumberSamplers;
	}

	SamplerStateDx12* DeviceState::GetSampler( const char* _pName ) const
	{
		const Interfaces::IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsVs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsVs->Samplers[i].Name ) ) == 0 )
			{
				return m_pSamplers[cbParamsVs->Samplers[i].BindPoint];
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsPs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsPs->Samplers[i].Name ) ) == 0 )
			{
				return m_pSamplers[cbParamsPs->Samplers[i].BindPoint];
			}
		}
		return nullptr;
	}

	GpuResourceDx12* DeviceState::GetTexture( const char* _pName ) const
	{
		const Interfaces::IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsVs->Textures[i].Name, _pName, ARRAYSIZE( cbParamsVs->Textures[i].Name ) ) == 0 )
			{
				return m_pTextures[cbParamsVs->Textures[i].BindPoint];
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsPs->Textures[i].Name, _pName, ARRAYSIZE( cbParamsPs->Textures[i].Name ) ) == 0 )
			{
				return m_pTextures[cbParamsPs->Textures[i].BindPoint];
			}
		}

		return nullptr;
	}

	GpuResourceDx12* DeviceState::GetConstantBuffer( const char* _pName ) const
	{
		const Interfaces::IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsVs->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsVs->Buffers[i].Name ) ) == 0 )
			{
				return m_pConstantBuffers[cbParamsVs->Buffers[i].BindPoint];
			}
		}

		const Interfaces::IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsPs->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsPs->Buffers[i].Name ) ) == 0 )
			{
				return m_pConstantBuffers[cbParamsPs->Buffers[i].BindPoint];
			}
		}
		return nullptr;
	}

	ShaderDx12* DeviceState::GetShader( Interfaces::IShaderStage::ShaderType _type )
	{
		switch ( _type )
		{
			case Interfaces::IShaderStage::ShaderType::EVertexShader:
				return m_pVertexShader;
			case Interfaces::IShaderStage::ShaderType::EPixelShader:
				return m_pPixelShader;
		}

		return nullptr;
	}
}
