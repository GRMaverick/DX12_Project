#ifndef __ShaderCache_h__
#define __ShaderCache_h__

#include <vector>
#include <d3d12.h>

#include "ShaderData.h"

class ShaderCache
{
public:
	ShaderCache(void);
	ShaderCache(const char* _pShadersPath);
	~ShaderCache(void);

	void Load(const char* _pCachePath);

	D3D12_SHADER_BYTECODE GetShader(const char* _pName);

private:

	std::vector<ShaderData> m_vLoadedShaders;
};

#endif // __ShaderCache_h__