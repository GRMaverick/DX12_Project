#include "Defines.h"

#include "ShaderCache.h"
#include "ShaderCompilerDXC.h"
#include "ShaderCompilerFXC.h"

#include <Windows.h>
#include <stdlib.h>

#include "SysUtilities/_Loaders/CLParser.h"

//#define DUMP_SHADERS

using namespace SysUtilities;

namespace SysRenderer
{
	using namespace Interfaces;

	namespace D3D12
	{
		ShaderCache::ShaderCache( void )
		{
		}

		ShaderCache::ShaderCache( const char* _pShaderPaths )
		{
			LogInfo( "Loading ShaderCache:" );

			InitCompiler();

			Load( _pShaderPaths );
		}

		ShaderCache::~ShaderCache( void )
		{
		}

		ShaderCache* ShaderCache::Instance( void )
		{
			static ShaderCache cache;
			return &cache;
		}

		void ShaderCache::InitCompiler( void )
		{
			m_pShaderCompiler = nullptr;

			if ( CLParser::Instance()->HasArgument( "dxc" ) )
			{
				LogInfo( "Compiling using: DXCompiler" );
				m_pShaderCompiler = new ShaderCompilerDXC();
			}
			else
			{
				LogInfo( "Compiling using: D3DCompiler" );
				m_pShaderCompiler = new ShaderCompilerFXC();
			}
		}

		bool ShaderCache::Load( const char* _pShadersPath )
		{
			if ( !m_pShaderCompiler )
			{
				InitCompiler();
				if ( !m_pShaderCompiler )
				{
					LogError( "\tInvalid Shader Compiler" );
					return false;
				}
			}

			WIN32_FIND_DATAA data = {};
			ZeroMemory( &data, sizeof(WIN32_FIND_DATAA) );

			HANDLE hFind = FindFirstFileA( _pShadersPath, &data );
			if ( hFind != INVALID_HANDLE_VALUE )
			{
				char* pDirectoryNoWildcard = new char[strlen( _pShadersPath )];
				strncpy_s( pDirectoryNoWildcard, strlen( _pShadersPath ), _pShadersPath, strlen( _pShadersPath ) - 1 );

				do
				{
					if ( strncmp( data.cFileName, ".", strlen( data.cFileName ) ) != 0 && strncmp( data.cFileName, "..", strlen( data.cFileName ) ) != 0 && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY )
					{
						char* pFullFilepath = new char[strlen( pDirectoryNoWildcard ) + strlen( data.cFileName ) + 1];
						snprintf( pFullFilepath, strlen( pDirectoryNoWildcard ) + strlen( data.cFileName ) + 1, "%s%s", pDirectoryNoWildcard, data.cFileName );

						char pFullFullFileString[128];
						GetFullPathNameA( pFullFilepath, ARRAYSIZE( pFullFullFileString ), pFullFullFileString, nullptr );

						LogInfo( "\t- %s", pFullFullFileString );

						size_t namelength = strlen( data.cFileName ) - 2;
						char*  pFilename  = new char[namelength];
						snprintf( pFilename, namelength, "%s", data.cFileName );

						char*         aError        = nullptr;
						IShaderStage* pVertexShader = m_pShaderCompiler->Compile( pFullFilepath, "MainVS", aError );
						if ( !pVertexShader )
						{
							LogError( "\t- %s failed to compile [Vertex Shader]", pFullFullFileString );
							continue;
						}

						char pVShaderName[32] = {0};
						snprintf( pVShaderName, ARRAYSIZE( pVShaderName ), "%s.vs", pFilename );
						pVertexShader->SetName( pVShaderName );
						m_pShaderCompiler->Reflect( pVertexShader );
						DumpShader( pVertexShader );

						IShaderStage* pPixelShader = m_pShaderCompiler->Compile( pFullFilepath, "MainPS", aError );
						if ( !pPixelShader )
						{
							LogError( "\t- %s failed to compile [Pixel Shader]", pFullFullFileString );
							continue;
						}

						char pPShaderName[32] = {0};
						snprintf( pPShaderName, ARRAYSIZE( pPShaderName ), "%s.ps", pFilename );
						pPixelShader->SetName( pPShaderName );
						m_pShaderCompiler->Reflect( pPixelShader );
						DumpShader( pPixelShader );

						Effect effect = Effect( pVertexShader, pPixelShader );
						effect.SetName( pFilename );

						m_vLoadedEffect.push_back( effect );

						delete[] pFilename;
						delete[] pFullFilepath;
					}
				}
				while ( FindNextFileA( hFind, &data ) );

				delete[] pDirectoryNoWildcard;

				FindClose( hFind );
			}

			return true;
		}

		void ShaderCache::DumpShader( IShaderStage* _pShader )
		{
#if defined(_DEBUG) && defined(DUMP_SHADERS)
			const ShaderIOParameters& parameters = _pShader->GetShaderParameters();

			LogInfo("\t\tInputs: %u", parameters.NumberInputs);
			for (unsigned int parameter = 0; parameter < parameters.NumberInputs; ++parameter)
			{
				ShaderIOParameters::Parameter& p = parameters.Inputs[parameter];
				LogInfo("\t\t\tSemanticName: %s", p.SemanticName);
				LogInfo("\t\t\t\tSemanticIndex: %u", p.SemanticIndex);
				LogInfo("\t\t\t\tRegister: %u", p.Register);
				LogInfo("\t\t\t\tComponentType: %u", p.ComponentType);
				LogInfo("\t\t\t\tSystemValueType: %u", p.SystemValueType);
			}

			LogInfo("\t\tOutputs: %u", parameters.NumberOutputs);
			for (unsigned int parameter = 0; parameter < parameters.NumberOutputs; ++parameter)
			{
				ShaderIOParameters::Parameter& p = parameters.Outputs[parameter];
				LogInfo("\t\t\tSemanticName: %s", p.SemanticName);
				LogInfo("\t\t\t\tSemanticIndex: %u", p.SemanticIndex);
				LogInfo("\t\t\t\tRegister: %u", p.Register);
				LogInfo("\t\t\t\tComponentType: %u", p.ComponentType);
				LogInfo("\t\t\t\tSystemValueType: %u", p.SystemValueType);
			}
#endif
		}

		Effect* ShaderCache::GetEffect( const char* _pName )
		{
			for ( size_t i = 0; i < m_vLoadedEffect.size(); ++i )
			{
				if ( strncmp( m_vLoadedEffect[i].GetName(), _pName, strlen( _pName ) ) == 0 )
				{
					return &m_vLoadedEffect[i];
				}
			}
			assert( "Effect does not exist" );
			return nullptr;
		}
	}
}
