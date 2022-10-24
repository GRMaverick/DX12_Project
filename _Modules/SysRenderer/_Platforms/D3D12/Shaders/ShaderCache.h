#ifndef __ShaderCache_h__
#define __ShaderCache_h__

#include <vector>

#include "IShader.h"

namespace SysRenderer
{
	namespace Interfaces
	{
		class IShaderCompiler;
	}

	namespace D3D12
	{
		struct ShaderSet
		{
			Interfaces::IShaderStage* VertexShader = nullptr;
			Interfaces::IShaderStage* PixelShader  = nullptr;
		};

		class ShaderCache
		{
		public:
			~ShaderCache( void );

			static ShaderCache* Instance( void );

			void InitCompiler( void );
			bool Load( const std::string& _pCachePath );

			Effect* GetEffect( const char* _pName );

		private:
			ShaderCache( void );
			explicit ShaderCache( const std::string& _pShadersPath );

			Interfaces::IShaderCompiler* m_pShaderCompiler;
			std::vector<Effect>          m_vLoadedEffect;

			static void DumpShader( Interfaces::IShaderStage* _pShader );
		};
	}
}

#endif // __ShaderCache_h__
