#ifndef __ShaderCompilerFXC_h__
#define __ShaderCompilerFXC_h__

#include "ShaderCompilerDX.h"

namespace ArtemisRenderer::Shaders
{
	class ShaderCompilerFXC final : public ShaderCompilerDX
	{
	public:
		ShaderCompilerFXC(void);
		~ShaderCompilerFXC(void);

		virtual IShaderStage*	Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) override final;
		virtual void			Reflect(IShaderStage* _pShader) override final;

	private:
	};
}

#endif // __ShaderCompilerFXC_h__