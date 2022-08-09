#ifndef __ShaderCompilerDXC_h__
#define __ShaderCompilerDXC_h__

#include "ShaderCompilerDX.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class ShaderCompilerDXC final : public ShaderCompilerDX
		{
		public:
			ShaderCompilerDXC(void);
			~ShaderCompilerDXC(void);

			virtual Interfaces::IShaderStage* Compile(const char* _pFilename, const char* _pFunctionName, char* _pError) override final;
			virtual void		Reflect(Interfaces::IShaderStage* _pShader) override final;

		protected:
			virtual const wchar_t* GetTargetProfile(const char* _pFilename) override final;

		private:
		};
	}
}

#endif // __ShaderCompilerDXC_h__