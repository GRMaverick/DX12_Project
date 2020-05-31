#ifndef __ShaderCompiler_h__
#define __ShaderCompiler_h__

struct ShaderData
{
public:
	ShaderData(void) { }
	~ShaderData(void) { }

	char			ShaderName[50];
	unsigned int	ShaderByteCodeSize;
	char*			ShaderByteCode;
};

class ShaderCompiler
{
public:

	ShaderCompiler(bool _bIsDxcCompiler = false);
	~ShaderCompiler(void);

	ShaderData Compile(const char* _pFilename, const char* _pFunctionName, const char* _pTarget, char* _pError);
	void Preprocess(const char* _pFilename);
	void Reflect(const char* _pFilename, const char* _pFunctionName);

private:
	bool m_bIsDxcCompiler;
};

#endif // __ShaderCompiler_h__