#ifndef __IShader_h__
#define __IShader_h__

struct ShaderIOParameters
{
	struct Parameter
	{
		char SemanticName[32] = { 0 };
		unsigned int Register = -1;
		unsigned int SemanticIndex = -1;
		unsigned int SystemValueType = -1;
		unsigned int ComponentType = -1;
		unsigned int Mask = -1;
	};

	size_t NumberInputs = 0;
	Parameter* Inputs = nullptr;

	size_t NumberOutputs = 0;
	Parameter* Outputs = nullptr;
};

struct ConstantTableReflection
{

};

class IShader
{
public:
	enum class ShaderType
	{
		VertexShader = 0,
		PixelShader,
	};

	~IShader(void) { }

	void SetName(const char* _pName) { strncpy_s(m_pShaderName, strlen(_pName) + 1, _pName, strlen(_pName)); }
	void SetShaderParameters(const ShaderIOParameters& _params) { m_Parameters = _params; }

	ShaderType GetType(void) { return m_Type; }
	const char* GetShaderName(void) { return m_pShaderName; }
	const void* GetBytecode(void) { return m_pShaderBytecode; }
	const size_t GetBytecodeSize(void) { return m_ShaderBytecodeSize; }
	const ShaderIOParameters GetParameters(void) { return m_Parameters; }

protected:
	ShaderType		m_Type;
	char			m_pShaderName[50];
	void*			m_pShaderBytecode;
	size_t			m_ShaderBytecodeSize;

	ShaderIOParameters m_Parameters;
};

#endif // __IShader_h__