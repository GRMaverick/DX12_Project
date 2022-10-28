#include "Dx12Defines.h"

#include "ShaderDx12.h"
#include "ShaderCompilerDXC.h"

#include <Windows.h>
#include <dxcapi.h>
#include <d3dcompiler.h>

#include <cstring>
#include <vector>

#include "Loaders/CLParser.h"

#pragma comment(lib, "dxcompiler.lib")

//#define DUMP_BLOBS

namespace Artemis::Renderer::Device::Dx12
{
	using namespace Interfaces;

	ShaderCompilerDxc::ShaderCompilerDxc( void )
	{
	}

	ShaderCompilerDxc::~ShaderCompilerDxc( void )
	{
	}

	const char* ExtractExtension( const char* _pFilename )
	{
		size_t       extensionStart = 0;
		const size_t size           = strlen( _pFilename );
		for ( size_t i = size - 1; i >= 0; --i )
		{
			if ( _pFilename[i] == '.' )
			{
				extensionStart = i + 1;
				break;
			}
		}

		char* ext = new char[5];
		snprintf( ext, 5, "%s", &_pFilename[extensionStart] );
		return ext;
	}

	const wchar_t* GetTargetProfile( const char* _pFilename )
	{
		if ( strstr( _pFilename, "MainVS" ) )
		{
			// Vertex Shader
			return L"vs_6_0";
		}
		else if ( strstr( _pFilename, "MainPS" ) )
		{
			// Pixel Shader
			return L"ps_6_0";
		}
		return L"";
	}

	IShaderStage* ShaderCompilerDxc::Compile( const char* _pFilename, const char* _pFunctionName, char* _pError )
	{
		IDxcLibrary* pLibrary = nullptr;
		VALIDATE_D3D( DxcCreateInstance( CLSID_DxcLibrary, IID_PPV_ARGS( &pLibrary ) ) );

		size_t   numRead       = 0;
		wchar_t* pFilenameWstr = new wchar_t[strlen( _pFilename )];
		mbstowcs_s( &numRead, pFilenameWstr, strlen( _pFilename ) + 1, _pFilename, strlen( _pFilename ) );

		wchar_t* pFunctionNameWstr = new wchar_t[strlen( _pFunctionName )];
		mbstowcs_s( &numRead, pFunctionNameWstr, strlen( _pFunctionName ) + 1, _pFunctionName, strlen( _pFunctionName ) );

		IDxcBlobEncoding* pBlob = nullptr;
		VALIDATE_D3D( pLibrary->CreateBlobFromFile( pFilenameWstr, 0, &pBlob ) );

		IDxcCompiler* pCompiler = nullptr;
		VALIDATE_D3D( DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( &pCompiler ) ) );

		const wchar_t* targetProfile = GetTargetProfile( _pFunctionName );

		std::vector<const wchar_t*> vArgs;

		if ( CLParser::Instance()->HasArgument( "pdbFat" ) )
		{
			vArgs.push_back( L"/Zi" );
		}
		else if ( CLParser::Instance()->HasArgument( "pdbSlim" ) )
		{
			vArgs.push_back( L"/Zs" );
		}

		if ( CLParser::Instance()->HasArgument( "Od" ) )
		{
			vArgs.push_back( L"/Od" );
		}
		else if ( CLParser::Instance()->HasArgument( "O3" ) )
		{
			vArgs.push_back( L"-O3" );
		}

		if ( CLParser::Instance()->HasArgument( "embed_symbols" ) )
		{
			vArgs.push_back( L"/Qembed_debug" );
		}

		LPCWSTR* args = vArgs.size() > 0 ? &vArgs[0] : nullptr;
		if ( !pBlob )
		{
			LogError( "%s failed to load", _pFilename );
			return nullptr;
		}

		IDxcIncludeHandler* pIncludeHandler = nullptr;
		pLibrary->CreateIncludeHandler( &pIncludeHandler );

		IDxcOperationResult* pOpsResult = nullptr;
		VALIDATE_D3D( pCompiler->Compile( pBlob, pFilenameWstr, pFunctionNameWstr, targetProfile, args, static_cast<UINT32>(vArgs.size()), nullptr, 0, pIncludeHandler, &pOpsResult ) );

		pIncludeHandler->Release();

		if ( !pOpsResult )
		{
			LogError( "%s failed to compile", _pFilename );
			return nullptr;
		}

		HRESULT status = S_OK;
		VALIDATE_D3D( pOpsResult->GetStatus( &status ) );
		if ( FAILED( status ) )
		{
			IDxcBlobEncoding* pError = nullptr;
			pOpsResult->GetErrorBuffer( &pError );

			const char* pErrorString = static_cast<const char*>(pError->GetBufferPointer());
			LogError( "Shader Compiler Error %s\n", pErrorString );
			pError->Release();
			return nullptr;
		}

		IDxcBlob* pCompiledCode = nullptr;
		VALIDATE_D3D( pOpsResult->GetResult( &pCompiledCode ) );

		if ( !pCompiledCode )
		{
			return nullptr;
		}

		IShaderStage* pShader = new ShaderDx12( pCompiledCode->GetBufferPointer(), pCompiledCode->GetBufferSize(), targetProfile[0] == L'v' ? IShaderStage::ShaderType::EVertexShader : IShaderStage::ShaderType::EPixelShader );

		pCompiledCode->Release();
		pOpsResult->Release();
		pCompiler->Release();
		pBlob->Release();
		pLibrary->Release();

		return pShader;
	}

	void ShaderCompilerDxc::Reflect( IShaderStage* _pShader )
	{
		IDxcLibrary* pLibrary = nullptr;
		VALIDATE_D3D( DxcCreateInstance( CLSID_DxcLibrary, IID_PPV_ARGS( &pLibrary ) ) );

		IDxcBlobEncoding* pBlob = nullptr;
		VALIDATE_D3D( pLibrary->CreateBlobWithEncodingOnHeapCopy( _pShader->GetBytecode(), static_cast<UINT32>(_pShader->GetBytecodeSize()), CP_ACP, &pBlob ) );

		IDxcContainerReflection* pReflectionCntr = nullptr;
		VALIDATE_D3D( DxcCreateInstance( CLSID_DxcContainerReflection, IID_PPV_ARGS( &pReflectionCntr ) ) );
		VALIDATE_D3D( pReflectionCntr->Load( pBlob ) );

#if defined(DUMP_BLOBS)
		UINT32 parts = 0;
		pReflectionCntr->GetPartCount(&parts);
		for (unsigned int part = 0; part < parts; ++part)
		{
			UINT32 kind = 0;
			VALIDATE_D3D(pReflectionCntr->GetPartKind(part, &kind));

			UINT32 shaderIdx;
			VALIDATE_D3D(pReflectionCntr->FindFirstPartKind(kind, &shaderIdx));

			IDxcBlob* pContent;
			VALIDATE_D3D(pReflectionCntr->GetPartContent(shaderIdx, &pContent));

			char CC4[5];
			CC4[0] = (char)(kind >> 0);
			CC4[1] = (char)(kind >> 8);
			CC4[2] = (char)(kind >> 16);
			CC4[3] = (char)(kind >> 24);
			CC4[4] = '\0';

			if (pContent)
			{
				LogInfo("%s: %u bytes", CC4, pContent->GetBufferSize());
				pContent->Release();
			}
		}
#endif

		UINT32 shaderIdx;
		VALIDATE_D3D( pReflectionCntr->FindFirstPartKind( DFCC_DXIL, &shaderIdx ) );

		ID3D12ShaderReflection* pReflection = nullptr;
		VALIDATE_D3D( pReflectionCntr->GetPartReflection( shaderIdx, IID_PPV_ARGS( &pReflection ) ) );

		ReflectInternal( _pShader, pReflection );
	}
}
