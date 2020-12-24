#ifndef __IShader_h__
#define __IShader_h__

struct ShaderReflection
{

};

struct ConstantTableReflection
{

};

class IShader
{
public:
	~IShader(void) { }

	void SetName(const char* _pName) { strncpy_s(m_pShaderName, strlen(_pName) + 1, _pName, strlen(_pName));  }
	const char* GetShaderName(void) { return m_pShaderName; }
	const void* GetBytecode(void) { return m_pShaderBytecode; }
	const size_t GetBytecodeSize(void) { return m_ShaderBytecodeSize; }

protected:
	char			m_pShaderName[50];
	void*			m_pShaderBytecode;
	size_t			m_ShaderBytecodeSize;
};

#endif // __IShader_h__