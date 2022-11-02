#ifndef __ShaderCompilerDXC_h__
#define __ShaderCompilerDXC_h__

#include "ShaderCompilerDX.h"

namespace Artemis::Renderer::Device::Dx12
{
	class ShaderCompilerDxc final : public ShaderCompilerDX
	{
	public:
		ShaderCompilerDxc( void );
		~ShaderCompilerDxc( void ) override;

		virtual Interfaces::IShaderStage* Compile( const char* _pFilename, const char* _pFunctionName, char* _pError ) override final;
		virtual void                      Reflect( Interfaces::IShaderStage* _pShader ) override final;

	private:
	};
}

#endif // __ShaderCompilerDXC_h__
