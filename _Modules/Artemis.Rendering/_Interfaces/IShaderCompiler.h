#ifndef __IShaderCompiler_h__
#define __IShaderCompiler_h__

namespace SysRenderer
{
	namespace Interfaces
	{
		class IShaderCompiler
		{
		public:
			virtual ~IShaderCompiler( void )
			{
			}

			virtual IShaderStage* Compile( const char* _pFilename, const char* _pFunctionName, char* _pError ) = 0;
			virtual void          Reflect( IShaderStage* _pShader ) = 0;

		private:
		};
	}
}

#endif // __IShaderCompiler_h__
