#ifndef __IShader_h__
#define __IShader_h__

#include <cstdio>
#include <stdlib.h>
#include <string.h>

//#include "D3D12\Resources\ConstantTable.h"

namespace Artemis::Renderer::Interfaces
{
	struct ShaderIoParameters
	{
		struct Parameter
		{
			char         SemanticName[32] = {0};
			unsigned int Register         = -1;
			unsigned int SemanticIndex    = -1;
			unsigned int SystemValueType  = -1;
			unsigned int ComponentType    = -1;
			unsigned int Mask             = -1;
		};

		unsigned int NumberInputs = 0;
		Parameter*   Inputs       = nullptr;

		unsigned int NumberOutputs = 0;
		Parameter*   Outputs       = nullptr;
	};

	class IConstantBufferParameters;

	class IShaderStage
	{
	public:
		enum class ShaderType
		{
			EVertexShader = 0,
			EPixelShader,
		};

		virtual ~IShaderStage( void );

		void SetName( const char* _pName ) { strncpy_s( m_pShaderName, _countof( m_pShaderName ), _pName, _countof( m_pShaderName ) ); }

		ShaderType  GetType( void ) const { return m_stType; }
		const char* GetShaderName( void ) const { return m_pShaderName; }
		const void* GetBytecode( void ) const { return m_pShaderBytecode; }
		size_t      GetBytecodeSize( void ) const { return m_ShaderBytecodeSize; }

		void SetShaderParameters( const ShaderIoParameters& _params ) { m_sipShaderParameters = _params; }

		void SetConstantParameters( IConstantBufferParameters* _params )
		{
			m_cbpConstantParameters = _params;
		}

		ShaderIoParameters         GetShaderParameters( void ) const { return m_sipShaderParameters; }
		IConstantBufferParameters* GetConstantParameters( void ) const { return m_cbpConstantParameters; }

	protected:
		ShaderType m_stType;
		char       m_pShaderName[50]    = {};
		void*      m_pShaderBytecode    = nullptr;
		size_t     m_ShaderBytecodeSize = 0;

		ShaderIoParameters         m_sipShaderParameters;
		IConstantBufferParameters* m_cbpConstantParameters;
	};
}
#endif // __IShader_h__
