#ifndef __ShaderCache_h__
#define __ShaderCache_h__

class ShaderCache
{
public:
	ShaderCache(void);
	ShaderCache(const char* _pShadersPath);
	~ShaderCache(void);

	void Build(const char* _pShadersPath);
	void Load(const char* _pCachePath);



private:
	ShaderCompiler m_Compiler;
};

#endif // __ShaderCache_h__