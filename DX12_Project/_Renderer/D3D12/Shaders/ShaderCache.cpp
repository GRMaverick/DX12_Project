#include "Defines.h"

#include "ShaderCache.h"
#include "ShaderCompilerDXC.h"
#include "ShaderCompilerFXC.h"

#include <Windows.h>
#include <stdlib.h>

#include "CLParser.h"

ShaderCache::ShaderCache(void)
{
}

ShaderCache::ShaderCache(const char* _pShaderPaths)
{
	LogInfo_Renderer("Loading ShaderCache:");

	InitCompiler();

	Load(_pShaderPaths);
}

ShaderCache::~ShaderCache(void)
{
}

void ShaderCache::InitCompiler(void)
{
	m_pShaderCompiler = nullptr;

	if (CLParser::Instance()->HasArgument("dxc"))
	{
		LogInfo_Renderer("\tDXCompiler");
		m_pShaderCompiler = new ShaderCompilerDXC();
	}
	else
	{
		LogInfo_Renderer("\tD3DCompiler");
		m_pShaderCompiler = new ShaderCompilerFXC();
	}
}

bool ShaderCache::Load(const char* _pShadersPath)
{
	if (!m_pShaderCompiler)
	{
		LogError_Renderer("\tInvalid Shader Compiler");
		return false;
	}

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
				LogInfo_Renderer("\t%s", data.cFileName);

				char* pFullFilepath = new char[strlen(pDirectoryNoWildcard) + strlen(data.cFileName) + 1];
				snprintf(
					pFullFilepath, 
					strlen(pDirectoryNoWildcard) + strlen(data.cFileName) + 1,
					"%s%s", 
					pDirectoryNoWildcard,
					data.cFileName
				);

				char* aError = nullptr;
				
				IShader* sd = m_pShaderCompiler->Compile(pFullFilepath, "main", aError);
				sd->SetName(data.cFileName);

				m_vLoadedShaders.push_back(sd);

				delete[] pFullFilepath;
			}
		} while (FindNextFileA(hFind, &data));

		delete[] pDirectoryNoWildcard;

		FindClose(hFind);
	}

	return true;
}

IShader* ShaderCache::GetShader(const char* _pName)
{
	for (size_t i = 0; i < m_vLoadedShaders.size(); ++i)
	{
		if (strcmp(_pName, m_vLoadedShaders[i]->GetShaderName()) == 0)
		{
			return m_vLoadedShaders[i];
		}
	}
	return nullptr;
}