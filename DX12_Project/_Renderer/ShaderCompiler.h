#ifndef __ShaderCompiler_h__
#define __ShaderCompiler_h__

#include "ShaderData.h"

class ShaderCompiler
{
public:

	ShaderCompiler(void);
	~ShaderCompiler(void);

	static ShaderData CompileD3D(const char* _pFilename, const char* _pFunctionName, char* _pError);
	static ShaderData CompileDXIL(const char* _pFilename, const char* _pFunctionName, char* _pError);

	void Preprocess(const char* _pFilename);
	void Reflect(const char* _pFilename, const char* _pFunctionName);

private:
	static const char* GetTargetProfileD3D(const char* _pFilename);
	static const wchar_t* GetTargetProfileDXIL(const char* _pFilename);
};

#endif // __ShaderCompiler_h__