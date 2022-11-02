#include "Dx12Defines.h"

#include "ShaderDx12.h"
#include "ShaderCompilerFXC.h"

#include <d3dcompiler.h>

#include <stdlib.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace Artemis::Renderer::Device::Dx12
{
	using namespace Interfaces;

	ShaderCompilerFxc::ShaderCompilerFxc( void )
	{
	}

	ShaderCompilerFxc::~ShaderCompilerFxc( void )
	{
	}

	const char* ExtractExtensionFxc( const char* _pFilename )
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

	const char* GetTargetProfile( const char* _pFilename )
	{
		if ( strstr( _pFilename, "MainVS" ) )
		{
			// Vertex Shader
			return "vs_5_1";
		}
		else if ( strstr( _pFilename, "MainPS" ) )
		{
			// Pixel Shader
			return "ps_5_1";
		}
		return "";
	}

	IShaderStage* ShaderCompilerFxc::Compile( const char* _pFilename, const char* _pFunctionName, char* _pError )
	{
		ID3DBlob* pError    = nullptr;
		ID3DBlob* pByteCode = nullptr;

		size_t   numRead       = 0;
		wchar_t* pFilenameWstr = new wchar_t[strlen( _pFilename )];
		mbstowcs_s( &numRead, pFilenameWstr, strlen( _pFilename ) + 1, _pFilename, strlen( _pFilename ) );

		const char* pStrTargetProfile = GetTargetProfile( _pFunctionName );

		constexpr UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		VALIDATE_D3D( D3DCompileFromFile(pFilenameWstr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _pFunctionName, pStrTargetProfile, compileFlags, 0, &pByteCode, &pError) );

		if ( pError )
		{
			_pError = new char[pError->GetBufferSize()];
			strncpy_s( _pError, pError->GetBufferSize(), static_cast<const char*>(pError->GetBufferPointer()), pError->GetBufferSize() );
			pError->Release();

			LogError( "\nD3DCompiler Error: %s", _pError );
			return nullptr;
		}

		IShaderStage* pShader = new ShaderDx12( pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), pStrTargetProfile[0] == 'v' ? IShaderStage::ShaderType::EVertexShader : IShaderStage::ShaderType::EPixelShader );

		pByteCode->Release();

		return pShader;
	}

	void ShaderCompilerFxc::Reflect( IShaderStage* _pShader )
	{
		ID3D12ShaderReflection* pShaderReflection = nullptr;
		VALIDATE_D3D( D3DReflect(_pShader->GetBytecode(), _pShader->GetBytecodeSize(), IID_ID3D12ShaderReflection, reinterpret_cast<void**>(&pShaderReflection)) );

		ReflectInternal( _pShader, pShaderReflection );
	}
}
