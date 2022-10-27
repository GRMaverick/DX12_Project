#ifndef __ShaderCompilerDX_h__
#define __ShaderCompilerDX_h__

#include "Interfaces/IShaderCompiler.h"

struct ID3D12ShaderReflection;

namespace Artemis::Renderer::Interfaces
{
	class IShaderStage;
}

namespace Artemis::Renderer::Shaders
{
	class ShaderCompilerDX : public Interfaces::IShaderCompiler
	{
	public:
		~ShaderCompilerDX( void ) override
		{
		}

	protected:
		void ReflectInternal( Interfaces::IShaderStage* _pShader, ID3D12ShaderReflection* _pReflection ) const;
	};
}
#endif // __ShaderCompilerDX_h__
