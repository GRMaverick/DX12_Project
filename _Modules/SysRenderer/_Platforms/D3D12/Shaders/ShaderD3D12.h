#ifndef __ShaderD3D12_h__
#define __ShaderD3D12_h__

#include "IShader.h"
#include "SysMemory/include/MemoryGlobalTracking.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class ShaderD3D12 final : public Interfaces::IShaderStage
		{
		public:
			ShaderD3D12( const void* _pBytecode, const size_t& _szBytecode, ShaderType _type )
			{
				m_stType             = _type;
				m_ShaderBytecodeSize = _szBytecode;

				m_pShaderBytecode = new char[m_ShaderBytecodeSize];
				memcpy_s( m_pShaderBytecode, m_ShaderBytecodeSize, _pBytecode, m_ShaderBytecodeSize );

#if defined(_DEBUG)
				SysMemory::MemoryGlobalTracking::RecordExplicitAllocation( SysMemory::MemoryContextCategory::EShader, m_pShaderBytecode, m_ShaderBytecodeSize );
#endif
			}

			~ShaderD3D12( void )
			{
#if defined(_DEBUG)
				SysMemory::MemoryGlobalTracking::RecordExplicitDellocation( m_pShaderBytecode );
#endif

				if ( m_pShaderBytecode )
					delete[] m_pShaderBytecode;
				m_pShaderBytecode = nullptr;
			}
		};
	}
}

#endif // __ShaderD3D12_h__
