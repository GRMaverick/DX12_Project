#include "Defines.h"

#include "ShaderCompilerDX.h"

#include <d3dcompiler.h>

void ShaderCompilerDX::ReflectInternal(IShader* _pShader, ID3D12ShaderReflection* _pReflection)
{
	if (_pReflection)
	{
		D3D12_SHADER_DESC desc{};
		VALIDATE_D3D(_pReflection->GetDesc(&desc));

		ShaderIOParameters reflectData;
		reflectData.NumberInputs = desc.InputParameters;
		reflectData.Inputs = new ShaderIOParameters::Parameter[reflectData.NumberInputs];
		for (unsigned int i = 0; i < reflectData.NumberInputs; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
			VALIDATE_D3D(_pReflection->GetInputParameterDesc(i, &paramDesc));

			ShaderIOParameters::Parameter& p = reflectData.Inputs[i];
			p.Register = paramDesc.Register;
			p.ComponentType = paramDesc.ComponentType;
			p.SemanticIndex = paramDesc.SemanticIndex;
			p.SystemValueType = paramDesc.SystemValueType;
			p.Mask = paramDesc.Mask;
			strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
		}

		reflectData.NumberOutputs = desc.OutputParameters;
		reflectData.Outputs = new ShaderIOParameters::Parameter[reflectData.NumberOutputs];
		for (unsigned int i = 0; i < reflectData.NumberOutputs; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
			VALIDATE_D3D(_pReflection->GetOutputParameterDesc(i, &paramDesc));

			ShaderIOParameters::Parameter& p = reflectData.Outputs[i];
			p.Register = paramDesc.Register;
			p.ComponentType = paramDesc.ComponentType;
			p.SemanticIndex = paramDesc.SemanticIndex;
			p.SystemValueType = paramDesc.SystemValueType;
			p.Mask = paramDesc.Mask;
			strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
		}

		_pShader->SetShaderParameters(reflectData);

		_pReflection->Release();
	}
}