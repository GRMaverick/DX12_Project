#include "Defines.h"

#include "ShaderD3D12.h"
#include "ShaderCompilerFXC.h"

#include <d3dcompiler.h>

#include <stdlib.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace SysUtilities;

namespace SysRenderer
{
	using namespace Interfaces;

	namespace D3D12
	{
		ShaderCompilerFXC::ShaderCompilerFXC(void)
		{

		}

		ShaderCompilerFXC::~ShaderCompilerFXC(void)
		{

		}

		const char* ExtractExtensionFXC(const char* _pFilename)
		{
			size_t extensionStart = 0;
			size_t size = strlen(_pFilename);
			for (size_t i = size - 1; i >= 0; --i)
			{
				if (_pFilename[i] == '.')
				{
					extensionStart = i + 1;
					break;
				}
			}

			char* ext = new char[5];
			snprintf(ext, 5, "%s", &_pFilename[extensionStart]);
			return ext;
		}

		const char* GetTargetProfile(const char* _pFilename)
		{
			if (strstr(_pFilename, "MainVS"))
			{
				// Vertex Shader
				return "vs_5_1";
			}
			else if (strstr(_pFilename, "MainPS"))
			{
				// Pixel Shader
				return "ps_5_1";
			}
			return "";
		}

		IShaderStage* ShaderCompilerFXC::Compile(const char* _pFilename, const char* _pFunctionName, char* _pError)
		{
			ID3DBlob* pError = nullptr;
			ID3DBlob* pByteCode = nullptr;

			size_t numRead = 0;
			wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
			mbstowcs_s(&numRead, pFilenameWstr, strlen(_pFilename) + 1, _pFilename, strlen(_pFilename));

			const char* pStrTargetProfile = GetTargetProfile(_pFunctionName);

			UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
			VALIDATE_D3D(D3DCompileFromFile(pFilenameWstr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _pFunctionName,
				pStrTargetProfile, compileFlags, 0, &pByteCode, &pError));

			if (pError)
			{
				_pError = new char[pError->GetBufferSize()];
				strncpy_s(_pError, pError->GetBufferSize(), (const char*)pError->GetBufferPointer(), pError->GetBufferSize());
				pError->Release();

				LogError("\nD3DCompiler Error: %s", _pError);
				return nullptr;
			}

			IShaderStage* pShader = new ShaderD3D12(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(),
				pStrTargetProfile[0] == 'v' ? IShaderStage::ShaderType::VertexShader : IShaderStage::ShaderType::PixelShader
			);

			pByteCode->Release();

			return pShader;
		}

		void ShaderCompilerFXC::Reflect(IShaderStage* _pShader)
		{
			ID3D12ShaderReflection* pShaderReflection = nullptr;
			VALIDATE_D3D(D3DReflect(_pShader->GetBytecode(), _pShader->GetBytecodeSize(), IID_ID3D12ShaderReflection, (void**)&pShaderReflection));

			ReflectInternal(_pShader, pShaderReflection);
		}
	}
}