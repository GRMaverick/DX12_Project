#ifndef __ShaderCache_h__
#define __ShaderCache_h__

#include <vector>
#include <d3d12.h>

#include "IShader.h"

class IShaderCompiler;

class ShaderCache
{
public:
	ShaderCache(void);
	ShaderCache(const char* _pShadersPath);
	~ShaderCache(void);

	void InitCompiler(void);
	bool Load(const char* _pCachePath);

	IShader* GetShader(const char* _pName);

private:
	IShaderCompiler*		m_pShaderCompiler;
	std::vector<IShader*>	m_vLoadedShaders;
};

#endif // __ShaderCache_h__