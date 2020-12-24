#include "Defines.h"

#include "ShaderD3D12.h"
#include "ShaderCompilerDXC.h"

#include <dxcapi.h>

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
	VALIDATE_D3D(pCompiler->Compile(pBlob, pFilenameWstr, pFunctionNameWstr, GetTargetProfile(_pFilename), args, _countof(args),
		nullptr, 0, nullptr, &pOpsResult));

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

	IShader* pShaderData = new ShaderD3D12(pCompiledCode->GetBufferPointer(), pCompiledCode->GetBufferSize());

	pCompiledCode->Release();
	pOpsResult->Release();
	pCompiler->Release();
	pBlob->Release();
	pLibrary->Release();

	return pShaderData;
}

void ShaderCompilerDXC::Reflect(IShader* _pShader)
{
}
