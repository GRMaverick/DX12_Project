#ifndef __ShaderD3D12_h__
#define __ShaderD3D12_h__

#include "IShader.h"

class ShaderD3D12 final : public IShader
{
public:
	ShaderD3D12(void* pBytecode, const size_t& _szBytecode, ShaderType _type)
	{
		m_Type = _type;
		m_ShaderBytecodeSize = _szBytecode;

		m_pShaderBytecode = new char[m_ShaderBytecodeSize];
		memcpy_s(m_pShaderBytecode, m_ShaderBytecodeSize, pBytecode, m_ShaderBytecodeSize);
	}

	~ShaderD3D12(void)
	{
		if (m_pShaderBytecode) delete[] m_pShaderBytecode; m_pShaderBytecode = nullptr;
	}
};

#endif // __ShaderD3D12_h__