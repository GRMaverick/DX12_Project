#include "Defines.h"

#include "ShaderCache.h"
#include "ShaderCompiler.h"

#include <Windows.h>
#include <stdlib.h>

ShaderCache::ShaderCache(void)
{

}

ShaderCache::ShaderCache(const char* _pShaderPaths)
{
	Load(_pShaderPaths);
}

ShaderCache::~ShaderCache(void)
{

}

void ShaderCache::Load(const char* _pShadersPath)
{
	WIN32_FIND_DATAA data = { };
	ZeroMemory(&data, sizeof(WIN32_FIND_DATAA));

	HANDLE hFind = FindFirstFileA(_pShadersPath, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		char* pDirectoryNoWildcard = new char[strlen(_pShadersPath)];
		strncpy_s(
			pDirectoryNoWildcard, 
			strlen(_pShadersPath), 
			_pShadersPath, 
			strlen(_pShadersPath) - 1
		);

		do
		{
			if (strncmp(data.cFileName, ".", strlen(data.cFileName)) != 0 && 
				strncmp(data.cFileName, "..", strlen(data.cFileName)) != 0)
			{
				DebugPrint("Loading %s", data.cFileName);

				char* pFullFilepath = new char[strlen(pDirectoryNoWildcard) + strlen(data.cFileName) + 1];
				snprintf(
					pFullFilepath, 
					strlen(pDirectoryNoWildcard) + strlen(data.cFileName) + 1,
					"%s%s", 
					pDirectoryNoWildcard,
					data.cFileName
				);

				char* aError = nullptr;
				ShaderData sd = ShaderCompiler::CompileDXIL(pFullFilepath, "main", aError);
				strncpy_s(sd.ShaderName, strlen(data.cFileName)+1, data.cFileName, strlen(data.cFileName));

				m_vLoadedShaders.push_back(sd);

				delete[] pFullFilepath;
			}
		} while (FindNextFileA(hFind, &data));

		delete[] pDirectoryNoWildcard;

		FindClose(hFind);
	}
}

D3D12_SHADER_BYTECODE ShaderCache::GetShader(const char* _pName)
{
	for (size_t i = 0; i < m_vLoadedShaders.size(); ++i)
	{
		if (strncmp(_pName, m_vLoadedShaders[i].ShaderName, ARRAYSIZE(m_vLoadedShaders[i].ShaderName)) == 0)
		{
			D3D12_SHADER_BYTECODE sbc = {};
			ZeroMemory(&sbc, sizeof(D3D12_SHADER_BYTECODE));
			sbc.BytecodeLength = m_vLoadedShaders[i].ShaderByteCodeSize;
			sbc.pShaderBytecode = m_vLoadedShaders[i].ShaderByteCode;
			return sbc;
		}
	}
	return {};
}