#ifndef __ShaderCache_h__
#define __ShaderCache_h__

#include <vector>
#include <d3d12.h>

#include "IShader.h"

class IShaderCompiler;

struct ShaderSet
{
	IShader* VertexShader = nullptr;
	IShader* PixelShader = nullptr;
};

class ShaderCache
{
public:
	ShaderCache(void);
	ShaderCache(const char* _pShadersPath);
	~ShaderCache(void);

	void InitCompiler(void);
	bool Load(const char* _pCachePath);

	ShaderSet GetShader(const char* _pName);

private:
	IShaderCompiler*		m_pShaderCompiler;
	std::vector<IShader*>	m_vLoadedShaders;

	void DumpShader(IShader* _pShader);
};

#endif // __ShaderCache_h__