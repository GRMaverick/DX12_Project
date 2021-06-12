#ifndef __IShaderCompiler_h__
#define __IShaderCompiler_h__

#include "IShader.h"

class IShaderCompiler
{
public:
	~IShaderCompiler(void) { }

	virtual IShaderStage*	Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) = 0;
	virtual void		Reflect(IShaderStage* _pShader) = 0;

protected:
	virtual const wchar_t* GetTargetProfile(const char* _pFilename) = 0;
private:
};

#endif // __IShaderCompiler_h__
