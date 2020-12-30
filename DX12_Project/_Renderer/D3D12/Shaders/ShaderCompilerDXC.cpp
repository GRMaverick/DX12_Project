#include "Defines.h"

#include "ShaderD3D12.h"
#include "ShaderCompilerDXC.h"

#include <dxcapi.h>
#include <d3dcompiler.h>

#include <stdlib.h>
#include <cstring>

#pragma comment(lib, "dxcompiler.lib")

//#define DUMP_BLOBS

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
	if (strstr(_pFilename, "Vertex"))
	{
		// Vertex Shader
		return L"vs_6_0";
	}
	else if (strstr(_pFilename, "Pixel"))
	{
		// Pixel Shader
		return L"ps_6_0";
	}
	return L"";
}

IShaderStage* ShaderCompilerDXC::Compile(const char* _pFilename, const char* _pFunctionName, char* _pError)
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

	const wchar_t* targetProfile = GetTargetProfile(_pFunctionName);

	LPCWSTR args[] = 
	{
		L"/Zi", 
		L"/Od",
	};

	if (!pBlob)
	{
		LogError_Renderer("%s failed to load", _pFilename);
		return nullptr;
	}

	IDxcIncludeHandler* pIncludeHandler = nullptr;
	pLibrary->CreateIncludeHandler(&pIncludeHandler);

	IDxcOperationResult* pOpsResult = nullptr;
	VALIDATE_D3D(pCompiler->Compile(pBlob, pFilenameWstr, pFunctionNameWstr, targetProfile, args, _countof(args), nullptr, 0, pIncludeHandler, &pOpsResult));

	pIncludeHandler->Release();

	if (!pOpsResult)
	{
		LogError_Renderer("%s failed to compile", _pFilename);
		return nullptr;
	}

	HRESULT status = S_OK;
	VALIDATE_D3D(pOpsResult->GetStatus(&status));
	//VALIDATE_D3D(status);

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

	IShaderStage* pShader = new ShaderD3D12(pCompiledCode->GetBufferPointer(), pCompiledCode->GetBufferSize(),
		targetProfile[0] == L'v' ? IShaderStage::ShaderType::VertexShader : IShaderStage::ShaderType::PixelShader
	);

	pCompiledCode->Release();
	pOpsResult->Release();
	pCompiler->Release();
	pBlob->Release();
	pLibrary->Release();

	return pShader;
}

void ShaderCompilerDXC::Reflect(IShaderStage* _pShader)
{
	IDxcLibrary* pLibrary = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&pLibrary)));

	IDxcBlobEncoding* pBlob = nullptr;
	VALIDATE_D3D(pLibrary->CreateBlobWithEncodingOnHeapCopy(_pShader->GetBytecode(), (UINT32)_pShader->GetBytecodeSize(), CP_ACP, &pBlob));
	
	IDxcContainerReflection* pReflectionCntr = nullptr;
	VALIDATE_D3D(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pReflectionCntr)));
	VALIDATE_D3D(pReflectionCntr->Load(pBlob));

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
			LogInfo_Renderer("%s: %u bytes", CC4, pContent->GetBufferSize());
			pContent->Release();
		}
	}
#endif
	
	UINT32 shaderIdx;
	VALIDATE_D3D(pReflectionCntr->FindFirstPartKind(DFCC_DXIL, &shaderIdx));

	ID3D12ShaderReflection* pReflection = nullptr;
	VALIDATE_D3D(pReflectionCntr->GetPartReflection(shaderIdx, IID_PPV_ARGS(&pReflection)));

	ReflectInternal(_pShader, pReflection);
}
