#ifndef __ShaderCompilerDXC_h__
#define __ShaderCompilerDXC_h__

#include "IShaderCompiler.h"

class ShaderCompilerDXC final : public IShaderCompiler
{
public:
	ShaderCompilerDXC(void);
	~ShaderCompilerDXC(void);

	virtual IShader*	Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) override final;
	virtual void		Reflect(IShader* _pShader) override final;
	
protected:
	virtual const wchar_t* GetTargetProfile(const char* _pFilename) override final;

private:
};

#endif // __ShaderCompilerDXC_h__