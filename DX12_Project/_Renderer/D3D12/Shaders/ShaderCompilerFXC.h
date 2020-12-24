#ifndef __ShaderCompilerFXC_h__
#define __ShaderCompilerFXC_h__

#include "ShaderCompilerDX.h"

class ShaderCompilerFXC final : public ShaderCompilerDX
{
public:
	ShaderCompilerFXC(void);
	~ShaderCompilerFXC(void);

	virtual IShader*	Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) override final;
	virtual void		Reflect(IShader* _pShader) override final;

protected:
	virtual const wchar_t* GetTargetProfile(const char* _pFilename) override final;

private:
};

#endif // __ShaderCompilerFXC_h__