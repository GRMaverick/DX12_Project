#ifndef __ShaderCompilerFXC_h__
#define __ShaderCompilerFXC_h__

#include "ShaderCompilerDX.h"

namespace Artemis::Renderer::Device::Dx12
{
	class ShaderCompilerFxc final : public ShaderCompilerDX
	{
	public:
		ShaderCompilerFxc( void );
		~ShaderCompilerFxc( void ) override;

		virtual Interfaces::IShaderStage* Compile( const char* _pFilename, const char* _pFunctionName, char* _pError ) override final;
		virtual void                      Reflect( Interfaces::IShaderStage* _pShader ) override final;

	private:
	};
}

#endif // __ShaderCompilerFXC_h__
