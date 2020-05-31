#include "Defines.h"

#define _CRT_SECURE_NO_WARNINGS

#include "ShaderCompiler.h"

#include <d3dcompiler.h>
#include <dxcapi.h>

#include <assert.h>


#pragma comment(lib, "d3dcompiler.lib")

ShaderCompiler::ShaderCompiler(bool _bIsDxcCompiler)
{
	m_bIsDxcCompiler = _bIsDxcCompiler;
}
ShaderCompiler::~ShaderCompiler(void)
{

}

ShaderData ShaderCompiler::Compile(const char* _pFilename, const char* _pFunctionName, const char* _pTarget, char* _pError)
{
	if (m_bIsDxcCompiler)
	{

	}
	else
	{
		ID3DBlob* pError = nullptr;
		ID3DBlob* pByteCode = nullptr;

		wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
		mbstowcs(pFilenameWstr, _pFilename, strlen(_pFilename));

		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		HRESULT hr = D3DCompileFromFile(pFilenameWstr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _pFunctionName,
			_pTarget, compileFlags, 0, &pByteCode, &pError);

		if (!pError)
		{
			assert(!_pError && "_pError is a pointer to something, this is will leak the memory that was previously assigned to it");
			_pError = new char[pError->GetBufferSize()];
			strncpy(_pError, (const char*)pError->GetBufferPointer(), pError->GetBufferSize());
		}
		else
		{
			ShaderData d;
			strncpy(d.ShaderName, _pFilename, ARRAYSIZE(d.ShaderName) - 1);
			strncpy(d.ShaderByteCode, (const char*)pByteCode->GetBufferPointer(), pByteCode->GetBufferSize());
			d.ShaderByteCodeSize = pByteCode->GetBufferSize();
			return d;
		}
	}
	return ShaderData();
}
void ShaderCompiler::Preprocess(const char* _pFilename)
{

}
void ShaderCompiler::Reflect(const char* _pFilename, const char* _pFunctionName)
{

}
