#ifndef __Shader_h__
#define __Shader_h__

#include "IShader.h"

namespace ArtemisRenderer::Shaders
{
	class Shader final : public IShaderStage
	{
	public:
		Shader(void* pBytecode, const size_t& _szBytecode, ShaderType _type)
		{
			m_Type = _type;
			m_ShaderBytecodeSize = _szBytecode;

			m_pShaderBytecode = new char[m_ShaderBytecodeSize];
			memcpy_s(m_pShaderBytecode, m_ShaderBytecodeSize, pBytecode, m_ShaderBytecodeSize);
		}

		~Shader(void)
		{
			if (m_pShaderBytecode) delete[] m_pShaderBytecode; m_pShaderBytecode = nullptr;
		}
	};
}

#endif // __Shader_h__