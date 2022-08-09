#ifndef __IShader_h__
#define __IShader_h__

#include <stdlib.h>
#include "D3D12\Resources\ConstantTable.h"

namespace SysRenderer
{
	namespace Interfaces
	{
		struct ShaderIOParameters
		{
			struct Parameter
			{
				char			SemanticName[32] = { 0 };
				unsigned int	Register = -1;
				unsigned int	SemanticIndex = -1;
				unsigned int	SystemValueType = -1;
				unsigned int	ComponentType = -1;
				unsigned int	Mask = -1;
			};

			unsigned int		NumberInputs = 0;
			Parameter* Inputs = nullptr;

			unsigned int		NumberOutputs = 0;
			Parameter* Outputs = nullptr;
		};

		class IShaderStage
		{
		public:
			enum class ShaderType
			{
				VertexShader = 0,
				PixelShader,
			};

			~IShaderStage(void) { }

			void SetName(const char* _pName) { strncpy_s(m_pShaderName, _countof(m_pShaderName), _pName, _countof(m_pShaderName)); }

			ShaderType GetType(void) { return m_Type; }
			const char* GetShaderName(void) { return m_pShaderName; }
			const void* GetBytecode(void) { return m_pShaderBytecode; }
			const size_t GetBytecodeSize(void) { return m_ShaderBytecodeSize; }

			void SetShaderParameters(const ShaderIOParameters& _params) { m_ShaderParameters = _params; }
			void SetConstantParameters(const D3D12::ConstantBufferParameters& _params) { m_ConstantParameters = _params; D3D12::ConstantTable::Instance()->CreateConstantBuffersEntries(_params); }

			const ShaderIOParameters GetShaderParameters(void) { return m_ShaderParameters; }
			const D3D12::ConstantBufferParameters GetConstantParameters(void) { return m_ConstantParameters; }

		protected:
			ShaderType		m_Type;
			char			m_pShaderName[50];
			void* m_pShaderBytecode;
			size_t			m_ShaderBytecodeSize;

			ShaderIOParameters m_ShaderParameters;
			D3D12::ConstantBufferParameters m_ConstantParameters;
		};
	}

	class Effect
	{
	public:
		void SetName(const char* _pName) { snprintf(m_pName, _countof(m_pName), "%s", _pName); }
		void SetVertexShader(Interfaces::IShaderStage* _pShader) { m_pVertexShader = _pShader; }
		void SetPixelShader(Interfaces::IShaderStage* _pShader) { m_pPixelShader = _pShader; }


		Interfaces::IShaderStage* GetVertexShader(void) { return m_pVertexShader; }
		Interfaces::IShaderStage* GetPixelShader(void) { return m_pPixelShader; }

		const char* GetName(void) { return m_pName; }

	private:
		char m_pName[32];

		Interfaces::IShaderStage* m_pVertexShader;
		Interfaces::IShaderStage* m_pPixelShader;
	};
}

#endif // __IShader_h__