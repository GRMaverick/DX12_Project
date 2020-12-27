#include "Defines.h"

#include "ShaderCompilerDX.h"

#include <d3dcompiler.h>

#include "D3D12\Resources\ConstantBufferParameters.h"

void ShaderCompilerDX::ReflectInternal(IShader* _pShader, ID3D12ShaderReflection* _pReflection)
{
	if (_pReflection)
	{
		D3D12_SHADER_DESC desc{};
		VALIDATE_D3D(_pReflection->GetDesc(&desc));

		ShaderIOParameters shaderIO;
		shaderIO.NumberInputs = desc.InputParameters;
		shaderIO.Inputs = new ShaderIOParameters::Parameter[shaderIO.NumberInputs];
		for (unsigned int i = 0; i < shaderIO.NumberInputs; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
			VALIDATE_D3D(_pReflection->GetInputParameterDesc(i, &paramDesc));

			ShaderIOParameters::Parameter& p = shaderIO.Inputs[i];
			p.Register = paramDesc.Register;
			p.ComponentType = paramDesc.ComponentType;
			p.SemanticIndex = paramDesc.SemanticIndex;
			p.SystemValueType = paramDesc.SystemValueType;
			p.Mask = paramDesc.Mask;
			strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
		}

		shaderIO.NumberOutputs = desc.OutputParameters;
		shaderIO.Outputs = new ShaderIOParameters::Parameter[shaderIO.NumberOutputs];
		for (unsigned int i = 0; i < shaderIO.NumberOutputs; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
			VALIDATE_D3D(_pReflection->GetOutputParameterDesc(i, &paramDesc));

			ShaderIOParameters::Parameter& p = shaderIO.Outputs[i];
			p.Register = paramDesc.Register;
			p.ComponentType = paramDesc.ComponentType;
			p.SemanticIndex = paramDesc.SemanticIndex;
			p.SystemValueType = paramDesc.SystemValueType;
			p.Mask = paramDesc.Mask;
			strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
		}

		ConstantBufferParameters cbInfo;
		cbInfo.NumberBuffers = desc.ConstantBuffers;
		if (cbInfo.NumberBuffers)
		{
			cbInfo.Buffers = new ConstantBufferParameters::ConstantBuffer[cbInfo.NumberBuffers];
			for (unsigned int cb = 0; cb < cbInfo.NumberBuffers; ++cb)
			{
				ID3D12ShaderReflectionConstantBuffer* pCBuffer = _pReflection->GetConstantBufferByIndex(cb);
				if (pCBuffer)
				{
					D3D12_SHADER_BUFFER_DESC sbDesc;
					VALIDATE_D3D(pCBuffer->GetDesc(&sbDesc));

#if defined(DUMP_CONSTANTS)
					LogInfo_Renderer("CB Name: %s", sbDesc.Name);
					LogInfo_Renderer("\tSize: %u", sbDesc.Size);
					LogInfo_Renderer("\tType: %u", sbDesc.Type);
					LogInfo_Renderer("\tVariables: %u", sbDesc.Variables);
#endif

					ConstantBufferParameters::ConstantBuffer& rCBuffer = cbInfo.Buffers[cb];
					strncpy_s(rCBuffer.Name, sbDesc.Name, ARRAYSIZE(rCBuffer.Name));
					rCBuffer.Size = sbDesc.Size;
					rCBuffer.Type = sbDesc.Type;
					rCBuffer.NumberVariables = sbDesc.Variables;

					if (rCBuffer.NumberVariables)
					{
						rCBuffer.Variables = new ConstantBufferParameters::Variable[rCBuffer.NumberVariables];
						for (unsigned int var = 0; var < sbDesc.Variables; ++var)
						{
							ID3D12ShaderReflectionVariable* pCBVariable = pCBuffer->GetVariableByIndex(var);
							if (pCBVariable)
							{
								D3D12_SHADER_VARIABLE_DESC varDesc;
								VALIDATE_D3D(pCBVariable->GetDesc(&varDesc));

#if defined(DUMP_CONSTANTS)
								LogInfo_Renderer("\tVar Name: %s", varDesc.Name);
								LogInfo_Renderer("\t\tSize: %u", varDesc.Size);
								LogInfo_Renderer("\t\tOffset: %u", varDesc.StartOffset);
#endif

								ConstantBufferParameters::Variable& rVariable = rCBuffer.Variables[var];
								strncpy_s(rVariable.Name, varDesc.Name, ARRAYSIZE(rVariable.Name));
								rVariable.Size = varDesc.Size;
								rVariable.Offset = varDesc.StartOffset;

							}
						}
					}
				}
			}
		}

		cbInfo.NumberResources = desc.BoundResources;
		if (cbInfo.NumberResources)
		{
			cbInfo.Resources = new ConstantBufferParameters::BoundResource[cbInfo.NumberResources];
			for (unsigned int br = 0; br < cbInfo.NumberResources; ++br)
			{
				D3D12_SHADER_INPUT_BIND_DESC ibDesc;
				VALIDATE_D3D(_pReflection->GetResourceBindingDesc(br, &ibDesc));

				ConstantBufferParameters::BoundResource& resource = cbInfo.Resources[br];
				strncpy_s(resource.Name, ibDesc.Name, ARRAYSIZE(resource.Name));
				resource.Type = ibDesc.Type;
				resource.BindPoint = ibDesc.BindPoint;
				resource.BindCount = ibDesc.BindCount;

#if defined(DUMP_CONSTANTS)
				LogInfo_Renderer("\tBR Name: %s", resource.Name);
				LogInfo_Renderer("\t\tType: %u", resource.Type);
				LogInfo_Renderer("\t\tBind Point: %u", resource.BindPoint);
				LogInfo_Renderer("\t\tBind Count: %u", resource.BindCount);
#endif
			}
		}

		_pReflection->Release();

		_pShader->SetShaderParameters(shaderIO);
		_pShader->SetConstantParameters(cbInfo);

	}
}