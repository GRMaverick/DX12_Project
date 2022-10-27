#include "GpuResourceTable.h"

#include "Interfaces/IShader.h"

#include "Interfaces/IConstantBufferParameters.h"
#include "ConstantBufferResource.h"

#include <map>
#include <assert.h>

#include "Cache/ConstantBufferParameters.h"

using namespace Artemis::Renderer::Interfaces;

namespace Artemis::Renderer::Device
{
	GpuResourceTable::GpuResourceTable( void )
	{
	}

	GpuResourceTable::GpuResourceTable( IShaderStage* _pVS, IShaderStage* _pPS )
	{
		Initialise( _pVS, _pPS );
	}

	GpuResourceTable::~GpuResourceTable( void )
	{
		Destroy();
	}

	void GpuResourceTable::Initialise( IShaderStage* _pVS, IShaderStage* _pPS )
	{
		if ( m_bIsInitialised )
			Destroy();

		m_pVertexShader = _pVS;
		m_pPixelShader  = _pPS;

		std::map<unsigned int, const char*> cbBinds;
		std::map<unsigned int, const char*> texBinds;
		std::map<unsigned int, const char*> sampBinds;

		const Shaders::ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS.NumberBuffers; ++i )
		{
			cbBinds[cbParamsVS.Buffers[i].BindPoint] = cbParamsVS.Buffers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsVS.NumberSamplers; ++i )
		{
			sampBinds[cbParamsVS.Samplers[i].BindPoint] = cbParamsVS.Samplers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsVS.NumberTextures; ++i )
		{
			texBinds[cbParamsVS.Textures[i].BindPoint] = cbParamsVS.Textures[i].Name;
		}

		const Shaders::ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS.NumberBuffers; ++i )
		{
			if ( cbBinds.find( cbParamsPS.Buffers[i].BindPoint ) == cbBinds.end() )
				cbBinds[cbParamsPS.Buffers[i].BindPoint] = cbParamsPS.Buffers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsPS.NumberSamplers; ++i )
		{
			if ( sampBinds.find( cbParamsPS.Samplers[i].BindPoint ) == sampBinds.end() )
				sampBinds[cbParamsPS.Samplers[i].BindPoint] = cbParamsPS.Samplers[i].Name;
		}
		for ( unsigned int i = 0; i < cbParamsPS.NumberTextures; ++i )
		{
			if ( texBinds.find( cbParamsPS.Textures[i].BindPoint ) == texBinds.end() )
				texBinds[cbParamsPS.Textures[i].BindPoint] = cbParamsPS.Textures[i].Name;
		}

		m_uiNumberSamplers        = static_cast<unsigned int>(sampBinds.size());
		m_uiNumberTextures        = static_cast<unsigned int>(texBinds.size());
		m_uiNumberConstantBuffers = static_cast<unsigned int>(cbBinds.size());

		m_pSamplers        = new ISamplerState*[m_uiNumberSamplers];
		m_pTextures        = new IGpuBufferResource*[m_uiNumberTextures];
		m_pConstantBuffers = new IGpuBufferResource*[m_uiNumberConstantBuffers];

		m_bIsInitialised = true;
	}

	void GpuResourceTable::Destroy( void )
	{
		if ( m_bIsInitialised )
		{
			const IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
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

	bool GpuResourceTable::SetTexture( const char* _pName, IGpuBufferResource* _pTexture ) const
	{
		const IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsVS->Textures[i].Name, _pName, ARRAYSIZE( cbParamsVS->Textures[i].Name ) ) == 0 )
			{
				m_pTextures[cbParamsVS->Textures[i].BindPoint] = _pTexture;
				return true;
			}
		}

		const IConstantBufferParameters* cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsPS->Textures[i].Name, _pName, ARRAYSIZE( cbParamsPS->Textures[i].Name ) ) == 0 )
			{
				m_pTextures[cbParamsPS->Textures[i].BindPoint] = _pTexture;
				return true;
			}
		}

		// Can ignore this CB Binding, Shaders aren't expecting it
		// assert(false && "SRV Resource Binding Failed");

		return false;
	}

	bool GpuResourceTable::SetConstantBuffer( const char* _pName, IGpuBufferResource* _pCBuffer ) const
	{
		const IConstantBufferParameters* cbParamsVS = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVS->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsVS->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsVS->Buffers[i].Name ) ) == 0 )
			{
				m_pConstantBuffers[cbParamsVS->Buffers[i].BindPoint] = _pCBuffer;
				return true;
			}
		}

		const IConstantBufferParameters* cbParamsPS = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPS->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsPS->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsPS->Buffers[i].Name ) ) == 0 )
			{
				m_pConstantBuffers[cbParamsPS->Buffers[i].BindPoint] = _pCBuffer;
				return true;
			}
		}

		// Can ignore this CB Binding, Shaders aren't expecting it
		//assert(false && "CBV Resource Binding Failed");

		return false;
	}

	bool GpuResourceTable::SetSamplerState( const char* _pName, ISamplerState* _state ) const
	{
		const IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsVs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsVs->Samplers[i].Name ) ) == 0 )
			{
				m_pSamplers[cbParamsVs->Samplers[i].BindPoint] = _state;
				return true;
			}
		}

		const IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsPs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsPs->Samplers[i].Name ) ) == 0 )
			{
				m_pSamplers[cbParamsPs->Samplers[i].BindPoint] = _state;
				return true;
			}
		}

		return false;
	}

	unsigned long GpuResourceTable::GetTextures( IGpuBufferResource*** _pppResources ) const
	{
		(*_pppResources) = m_pTextures;
		return m_uiNumberTextures;
	}

	unsigned long GpuResourceTable::GetConstantBuffers( IGpuBufferResource*** _pppResources ) const
	{
		(*_pppResources) = m_pConstantBuffers;
		return m_uiNumberConstantBuffers;
	}

	unsigned long GpuResourceTable::GetSamplers( ISamplerState*** _ppResources ) const
	{
		(*_ppResources) = m_pSamplers;
		return m_uiNumberSamplers;
	}

	ISamplerState* GpuResourceTable::GetSampler( const char* _pName ) const
	{
		const IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberSamplers; ++i )
		{
			char targetName[32] = {0};
			snprintf( targetName, ARRAYSIZE( targetName ), "%sSampler", _pName );
			if ( strncmp( cbParamsVs->Samplers[i].Name, targetName, ARRAYSIZE( cbParamsVs->Samplers[i].Name ) ) == 0 )
			{
				return m_pSamplers[cbParamsVs->Samplers[i].BindPoint];
			}
		}

		const IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
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

	Interfaces::IGpuBufferResource* GpuResourceTable::GetTexture( const char* _pName ) const
	{
		const IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsVs->Textures[i].Name, _pName, ARRAYSIZE( cbParamsVs->Textures[i].Name ) ) == 0 )
			{
				return m_pTextures[cbParamsVs->Textures[i].BindPoint];
			}
		}

		const IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberTextures; ++i )
		{
			if ( strncmp( cbParamsPs->Textures[i].Name, _pName, ARRAYSIZE( cbParamsPs->Textures[i].Name ) ) == 0 )
			{
				return m_pTextures[cbParamsPs->Textures[i].BindPoint];
			}
		}

		return nullptr;
	}

	Interfaces::IGpuBufferResource* GpuResourceTable::GetConstantBuffer( const char* _pName ) const
	{
		const IConstantBufferParameters* cbParamsVs = m_pVertexShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsVs->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsVs->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsVs->Buffers[i].Name ) ) == 0 )
			{
				return m_pConstantBuffers[cbParamsVs->Buffers[i].BindPoint];
			}
		}

		const IConstantBufferParameters* cbParamsPs = m_pPixelShader->GetConstantParameters();
		for ( unsigned int i = 0; i < cbParamsPs->NumberBuffers; ++i )
		{
			if ( strncmp( cbParamsPs->Buffers[i].Name, _pName, ARRAYSIZE( cbParamsPs->Buffers[i].Name ) ) == 0 )
			{
				return m_pConstantBuffers[cbParamsPs->Buffers[i].BindPoint];
			}
		}

		return nullptr;
	}
}