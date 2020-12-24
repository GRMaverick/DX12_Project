#include "Defines.h"

#include "ShaderCache.h"
#include "ShaderCompilerDXC.h"
#include "ShaderCompilerFXC.h"

#include <Windows.h>
#include <stdlib.h>

#include "CLParser.h"

//#define DUMP_SHADERS

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
				
				IShader* pShader = m_pShaderCompiler->Compile(pFullFilepath, "main", aError);
				pShader->SetName(data.cFileName);

				m_pShaderCompiler->Reflect(pShader);
				m_vLoadedShaders.push_back(pShader);

				DumpShader(pShader);

				delete[] pFullFilepath;
			}
		} while (FindNextFileA(hFind, &data));

		delete[] pDirectoryNoWildcard;

		FindClose(hFind);
	}

	return true;
}

void ShaderCache::DumpShader(IShader* _pShader)
{
#if defined(_DEBUG) && defined(DUMP_SHADERS)
	const ShaderIOParameters& parameters = _pShader->GetParameters();

	LogInfo_Renderer("\t\tInputs: %u", parameters.NumberInputs);
	for (unsigned int parameter = 0; parameter < parameters.NumberInputs; ++parameter)
	{
		ShaderIOParameters::Parameter& p = parameters.Inputs[parameter];
		LogInfo_Renderer("\t\t\tSemanticName: %s", p.SemanticName);
		LogInfo_Renderer("\t\t\t\tSemanticIndex: %u", p.SemanticIndex);
		LogInfo_Renderer("\t\t\t\tRegister: %u", p.Register);
		LogInfo_Renderer("\t\t\t\tComponentType: %u", p.ComponentType);
		LogInfo_Renderer("\t\t\t\tSystemValueType: %u", p.SystemValueType);
	}

	LogInfo_Renderer("\t\tOutputs: %u", parameters.NumberOutputs);
	for (unsigned int parameter = 0; parameter < parameters.NumberOutputs; ++parameter)
	{
		ShaderIOParameters::Parameter& p = parameters.Outputs[parameter];
		LogInfo_Renderer("\t\t\tSemanticName: %s", p.SemanticName);
		LogInfo_Renderer("\t\t\t\tSemanticIndex: %u", p.SemanticIndex);
		LogInfo_Renderer("\t\t\t\tRegister: %u", p.Register);
		LogInfo_Renderer("\t\t\t\tComponentType: %u", p.ComponentType);
		LogInfo_Renderer("\t\t\t\tSystemValueType: %u", p.SystemValueType);
	}
#endif
}

ShaderSet ShaderCache::GetShader(const char* _pName)
{
	ShaderSet set;
	for (size_t i = 0; i < m_vLoadedShaders.size(); ++i)
	{
		const char* pName = m_vLoadedShaders[i]->GetShaderName();
		if (strstr(pName, _pName))
		{
			if (m_vLoadedShaders[i]->GetType() == IShader::ShaderType::VertexShader)
			{
				set.VertexShader = m_vLoadedShaders[i];
				continue;
			}

			if (m_vLoadedShaders[i]->GetType() == IShader::ShaderType::PixelShader)
			{
				set.PixelShader = m_vLoadedShaders[i];
				continue;
			}
		}
	}
	return set;
}