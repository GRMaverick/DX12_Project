#include "Defines.h"

#include "ShaderD3D12.h"
#include "ShaderCompilerDXC.h"

#include <dxcapi.h>
#include <d3dcompiler.h>

#include <stdlib.h>

#pragma comment(lib, "dxcompiler.lib")

ShaderCompilerDXC::ShaderCompilerDXC(void)
{

}

ShaderCompilerDXC::~ShaderCompilerDXC(void)
{

}

const char* ExtractExtension(const char* _pFilename)
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

const wchar_t* ShaderCompilerDXC::GetTargetProfile(const char* _pFilename)
{
	const char* pExtractedFileExtension = ExtractExtension(_pFilename);
	if (strncmp(pExtractedFileExtension, "vs", strlen(pExtractedFileExtension)) == 0)
	{
		// Vertex Shader
		delete pExtractedFileExtension;
		return L"vs_6_0";
	}
	else if (strncmp(pExtractedFileExtension, "ps", strlen(pExtractedFileExtension)) == 0)
	{
		// Pixel Shader
		delete pExtractedFileExtension;
		return L"ps_6_0";
	}
	delete pExtractedFileExtension;
	return L"";
}

IShader* ShaderCompilerDXC::Compile(const char* _pFilename, const char* _pFunctionName, char* _pError)
{
	IDxcLibrary* pLibrary = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&pLibrary)));

	size_t numRead = 0;
	wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
	mbstowcs_s(&numRead, pFilenameWstr, strlen(_pFilename) + 1, _pFilename, strlen(_pFilename));

	wchar_t* pFunctionNameWstr = new wchar_t[strlen(_pFunctionName)];
	mbstowcs_s(&numRead, pFunctionNameWstr, strlen(_pFunctionName) + 1, _pFunctionName, strlen(_pFunctionName));

	IDxcBlobEncoding* pBlob = nullptr;
	VALIDATE_D3D(pLibrary->CreateBlobFromFile(pFilenameWstr, 0, &pBlob));

	IDxcCompiler* pCompiler = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler)));

	const wchar_t* targetProfile = GetTargetProfile(_pFilename);

	LPCWSTR args[] = 
	{
		L"/Zi", L"/Od"
	};

	if (!pBlob)
	{
		LogError_Renderer("%s failed to load", _pFilename);
		return nullptr;
	}

	IDxcOperationResult* pOpsResult = nullptr;
	VALIDATE_D3D(pCompiler->Compile(pBlob, pFilenameWstr, pFunctionNameWstr, targetProfile, args, _countof(args), nullptr, 0, nullptr, &pOpsResult));

	if (!pOpsResult)
	{
		LogError_Renderer("%s failed to compile", _pFilename);
		return nullptr;
	}

	HRESULT status = S_OK;
	VALIDATE_D3D(pOpsResult->GetStatus(&status));
	VALIDATE_D3D(status);

	if (FAILED(status))
	{
		IDxcBlobEncoding* pError = nullptr;
		pOpsResult->GetErrorBuffer(&pError);

		const char* pErrorString = (const char*)pError->GetBufferPointer();
		LogError_Renderer("Shader Compiler Error %s\n", pErrorString);
		pError->Release();
		return nullptr;
	}

	IDxcBlob* pCompiledCode = nullptr;
	VALIDATE_D3D(pOpsResult->GetResult(&pCompiledCode));

	if (!pCompiledCode)
	{
		return nullptr;
	}

	IShader* pShader = new ShaderD3D12(pCompiledCode->GetBufferPointer(), pCompiledCode->GetBufferSize(),
		targetProfile[0] == L'v' ? IShader::ShaderType::VertexShader : IShader::ShaderType::PixelShader
	);

	pCompiledCode->Release();
	pOpsResult->Release();
	pCompiler->Release();
	pBlob->Release();
	pLibrary->Release();

	return pShader;
}

void ShaderCompilerDXC::Reflect(IShader* _pShader)
{
	IDxcLibrary* pLibrary = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&pLibrary)));

	IDxcBlobEncoding* pBlob = nullptr;
	VALIDATE_D3D(pLibrary->CreateBlobWithEncodingOnHeapCopy(_pShader->GetBytecode(), _pShader->GetBytecodeSize(), CP_ACP, &pBlob));
	
	IDxcContainerReflection* pReflectionCntr = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pReflectionCntr)));
	VALIDATE_D3D(pReflectionCntr->Load(pBlob));

	UINT32 shaderIdx;
	VALIDATE_D3D(pReflectionCntr->FindFirstPartKind(DFCC_DXIL, &shaderIdx));

	ID3D12ShaderReflection* pReflection = nullptr;
	VALIDATE_D3D(pReflectionCntr->GetPartReflection(shaderIdx, IID_PPV_ARGS(&pReflection)));

	ReflectInternal(_pShader, pReflection);
}
