#ifndef __IShader_h__
#define __IShader_h__

#include <stdlib.h>
#include "D3D12\Resources\ConstantTable.h"

namespace SysRenderer
{
	namespace Interfaces
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

		class IShaderStage
		{
		public:
			enum class ShaderType
			{
				EVertexShader = 0,
				EPixelShader,
			};

			~IShaderStage( void )
			{
			}

			void SetName( const char* _pName ) { strncpy_s( m_pShaderName, _countof( m_pShaderName ), _pName, _countof( m_pShaderName ) ); }

			ShaderType  GetType( void ) const { return m_stType; }
			const char* GetShaderName( void ) const { return m_pShaderName; }
			const void* GetBytecode( void ) const { return m_pShaderBytecode; }
			size_t      GetBytecodeSize( void ) const { return m_ShaderBytecodeSize; }

			void SetShaderParameters( const ShaderIoParameters& _params ) { m_sipShaderParameters = _params; }

			void SetConstantParameters( const D3D12::ConstantBufferParameters& _params )
			{
				m_cbpConstantParameters = _params;
				D3D12::ConstantTable::Instance()->CreateConstantBuffersEntries( _params );
			}

			ShaderIoParameters              GetShaderParameters( void ) const { return m_sipShaderParameters; }
			D3D12::ConstantBufferParameters GetConstantParameters( void ) const { return m_cbpConstantParameters; }

		protected:
			ShaderType m_stType;
			char       m_pShaderName[50]    = {};
			void*      m_pShaderBytecode    = nullptr;
			size_t     m_ShaderBytecodeSize = 0;

			ShaderIoParameters              m_sipShaderParameters;
			D3D12::ConstantBufferParameters m_cbpConstantParameters;
		};
	}

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

		Interfaces::IShaderStage* m_pVertexShader;
		Interfaces::IShaderStage* m_pPixelShader;
	};
}

#endif // __IShader_h__