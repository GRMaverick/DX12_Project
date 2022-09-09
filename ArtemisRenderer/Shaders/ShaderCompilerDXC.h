#ifndef __ShaderCompilerDXC_h__
#define __ShaderCompilerDXC_h__

#include "ShaderCompilerDX.h"

namespace ArtemisRenderer::Shaders
{
	class ShaderCompilerDXC final : public ShaderCompilerDX
	{
	public:
		ShaderCompilerDXC(void);
		~ShaderCompilerDXC(void);

		virtual IShaderStage*	Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) override final;
		virtual void			Reflect(IShaderStage* _pShader) override final;

	private:
	};
}

#endif // __ShaderCompilerDXC_h__