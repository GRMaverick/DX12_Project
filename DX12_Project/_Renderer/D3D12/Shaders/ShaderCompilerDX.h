#ifndef __ShaderCompilerDX_h__
#define __ShaderCompilerDX_h__

#include "IShaderCompiler.h"

struct ID3D12ShaderReflection;

class ShaderCompilerDX : public IShaderCompiler
{
public:
	virtual ~ShaderCompilerDX(void) { }

protected:
	void ReflectInternal(IShader* _pShader, ID3D12ShaderReflection* _pReflection);
};
#endif // __ShaderCompilerDX_h__