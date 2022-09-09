#ifndef __ShaderCache_h__
#define __ShaderCache_h__

#include <vector>
#include <d3d12.h>

#include "IShader.h"

namespace ArtemisRenderer::Shaders
{
	class IShaderCompiler;

	struct ShaderSet
	{
		IShaderStage* VertexShader = nullptr;
		IShaderStage* PixelShader = nullptr;
	};

	class ShaderCache
	{
	public:
		~ShaderCache(void);

		static ShaderCache* Instance(void);

		void InitCompiler(void);
		bool Load(const char* _pCachePath);

		Effect* GetEffect(const char* _pName);

	private:
		ShaderCache(void);
		ShaderCache(const char* _pShadersPath);

		IShaderCompiler* m_pShaderCompiler;
		std::vector<Effect>		m_vLoadedEffect;

		void DumpShader(IShaderStage* _pShader);
	};
}

#endif // __ShaderCache_h__