#pragma once

#include <cstdio>
#include <stdlib.h>

namespace Artemis::Renderer::Interfaces
{
	class IShaderStage;
}

namespace Artemis::Renderer::Shaders
{
	class Interfaces::IShaderStage;

	class Effect
	{
	public:
		Effect( Interfaces::IShaderStage* _pVertexShader, Interfaces::IShaderStage* _pPixelShader ) :
			m_pName{},
			m_pVertexShader( _pVertexShader ),
			m_pPixelShader( _pPixelShader )
		{
		}

		void SetName( const char* _pName ) { snprintf( m_pName, _countof( m_pName ), "%s", _pName ); }
		void SetVertexShader( Interfaces::IShaderStage* _pShader ) { m_pVertexShader = _pShader; }
		void SetPixelShader( Interfaces::IShaderStage* _pShader ) { m_pPixelShader = _pShader; }

		Interfaces::IShaderStage* GetVertexShader( void ) const { return m_pVertexShader; }
		Interfaces::IShaderStage* GetPixelShader( void ) const { return m_pPixelShader; }

		const char* GetName( void ) { return m_pName; }

	private:
		char m_pName[32];

		Artemis::Renderer::Interfaces::IShaderStage* m_pVertexShader;
		Artemis::Renderer::Interfaces::IShaderStage* m_pPixelShader;
	};
}
