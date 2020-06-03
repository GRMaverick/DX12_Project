#include "Defines.h"

#include "ShaderCompiler.h"

#include <d3dcompiler.h>
#include <dxcapi.h>

#include <stdlib.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")

ShaderCompiler::ShaderCompiler(void)
{

}

ShaderCompiler::~ShaderCompiler(void)
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
			extensionStart = i+1;
			break;
		}
	}

	char ext[5];
	snprintf(ext, ARRAYSIZE(ext), "%s", &_pFilename[extensionStart]);
	return ext;
}

const char* ShaderCompiler::GetTargetProfileD3D(const char* _pFilename)
{
	const char* pExtractedFileExtension = ExtractExtension(_pFilename);
	if (strncmp(pExtractedFileExtension, "vs", strlen(pExtractedFileExtension)) == 0)
	{
		// Vertex Shader
		return "vs_5_1";
	}
	else if (strncmp(pExtractedFileExtension, "ps", strlen(pExtractedFileExtension)) == 0)
	{
		// Pixel Shader
		return "ps_5_1";
	}
	return "";
}

const wchar_t* ShaderCompiler::GetTargetProfileDXIL(const char* _pFilename)
{
	const char* pExtractedFileExtension = ExtractExtension(_pFilename);
	if (strncmp(pExtractedFileExtension, "vs", strlen(pExtractedFileExtension)) == 0)
	{
		// Vertex Shader
		return L"vs_6_0";
	}
	else if (strncmp(pExtractedFileExtension, "ps", strlen(pExtractedFileExtension)) == 0)
	{
		// Pixel Shader
		return L"ps_6_0";
	}
	return L"";
}

ShaderData ShaderCompiler::CompileD3D(const char* _pFilename, const char* _pFunctionName, char* _pError)
{
	ID3DBlob* pError = nullptr;
	ID3DBlob* pByteCode = nullptr;

	size_t numRead = 0;
	wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
	mbstowcs_s(&numRead, pFilenameWstr, strlen(_pFilename)+1, _pFilename, strlen(_pFilename));

	const char* pTargetProfile = GetTargetProfileD3D(_pFilename);

	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	VALIDATE_D3D(D3DCompileFromFile(pFilenameWstr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _pFunctionName,
		pTargetProfile, compileFlags, 0, &pByteCode, &pError));

	if (pError)
	{
		_pError = new char[pError->GetBufferSize()];
		strncpy_s(_pError, pError->GetBufferSize(), (const char*)pError->GetBufferPointer(), pError->GetBufferSize());
	}
	else
	{
		ShaderData d;
		d.ShaderByteCode = new char[pByteCode->GetBufferSize()];
		strncpy_s(d.ShaderByteCode, pByteCode->GetBufferSize()+1, (const char*)pByteCode->GetBufferPointer(), pByteCode->GetBufferSize());
		d.ShaderByteCodeSize = pByteCode->GetBufferSize();
		return d;
	}

	return ShaderData();
}

ShaderData ShaderCompiler::CompileDXIL(const char* _pFilename, const char* _pFunctionName, char* _pError)
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

	IDxcOperationResult* pOpsResult = nullptr;
	VALIDATE_D3D(pCompiler->Compile(pBlob, pFilenameWstr, pFunctionNameWstr, GetTargetProfileDXIL(_pFilename),
		nullptr, 0, nullptr, 0, nullptr, &pOpsResult));

	if (!pOpsResult)
	{
		return ShaderData();
	}

	HRESULT status = S_OK;
	VALIDATE_D3D(pOpsResult->GetStatus(&status));
	VALIDATE_D3D(status);

	IDxcBlob* pCompiledCode = nullptr;
	VALIDATE_D3D(pOpsResult->GetResult(&pCompiledCode));

	if (!pCompiledCode)
	{
		return ShaderData();
	}

	ShaderData sd;
	sd.ShaderByteCodeSize = pCompiledCode->GetBufferSize();
	sd.ShaderByteCode = new char[sd.ShaderByteCodeSize];
	memcpy_s(sd.ShaderByteCode, sd.ShaderByteCodeSize + 1, pCompiledCode->GetBufferPointer(), sd.ShaderByteCodeSize);

	pCompiledCode->Release();
	pOpsResult->Release();
	pCompiler->Release();
	pBlob->Release();
	pLibrary->Release();

	return sd;
}

void ShaderCompiler::Preprocess(const char* _pFilename)
{

}
void ShaderCompiler::Reflect(const char* _pFilename, const char* _pFunctionName)
{

}
