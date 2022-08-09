#ifndef __ShaderCompilerDX_h__
#define __ShaderCompilerDX_h__

#include "IShaderCompiler.h"

struct ID3D12ShaderReflection;

namespace SysRenderer
{
	namespace D3D12
	{
		class ShaderCompilerDX : public Interfaces::IShaderCompiler
		{
		public:
			virtual ~ShaderCompilerDX(void) { }

		protected:
			void ReflectInternal(Interfaces::IShaderStage* _pShader, ID3D12ShaderReflection* _pReflection);
		};
	}
}
#endif // __ShaderCompilerDX_h__