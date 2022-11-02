#ifndef __ShaderD3D12_h__
#define __ShaderD3D12_h__

#include "Interfaces/IShader.h"
#include "Memory/MemoryGlobalTracking.h"

namespace Artemis::Renderer::Device::Dx12
{
	class ShaderDx12 final : public Artemis::Renderer::Interfaces::IShaderStage
	{
	public:
		ShaderDx12( const void* _pBytecode, const size_t& _szBytecode, const ShaderType _type )
		{
			m_stType             = _type;
			m_ShaderBytecodeSize = _szBytecode;

			m_pShaderBytecode = new char[m_ShaderBytecodeSize];
			memcpy_s( m_pShaderBytecode, m_ShaderBytecodeSize, _pBytecode, m_ShaderBytecodeSize );

#if defined(_DEBUG)
			Memory::MemoryGlobalTracking::RecordExplicitAllocation( Memory::MemoryContextCategory::EShader, m_pShaderBytecode, m_ShaderBytecodeSize );
#endif
		}

		~ShaderDx12( void ) override
		{
#if defined(_DEBUG)
			Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_pShaderBytecode );
#endif

			if ( m_pShaderBytecode )
				delete[] m_pShaderBytecode;
			m_pShaderBytecode = nullptr;
		}
	};
}

#endif // __ShaderD3D12_h__
