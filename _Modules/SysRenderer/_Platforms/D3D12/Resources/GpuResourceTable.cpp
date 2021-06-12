#include "GpuResourceTable.h"

#include "IShader.h"

#include "ConstantBufferParameters.h"
#include "ConstantBufferResource.h"

#include <map>
#include <assert.h>

GpuResourceTable::GpuResourceTable(void)
{
}

GpuResourceTable::GpuResourceTable(IShaderStage* _pVS, IShaderStage* _pPS)
{
	Initialise(_pVS, _pPS);
}

GpuResourceTable::~GpuResourceTable(void)
{
	Destroy();
}

void GpuResourceTable::Initialise(IShaderStage* _pVS, IShaderStage* _pPS)
{
	if (m_bIsInitialised)
		Destroy();

	m_pVertexShader = _pVS;
	m_pPixelShader = _pPS;

	std::map<unsigned int, const char*> cbBinds;
	std::map<unsigned int, const char*> texBinds;
	std::map<unsigned int, const char*> sampBinds;

	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsVS.NumberBuffers; ++i)
	{
		cbBinds[cbParamsVS.Buffers[i].BindPoint] = cbParamsVS.Buffers[i].Name;
	}
	for (unsigned int i = 0; i < cbParamsVS.NumberSamplers; ++i)
	{
		sampBinds[cbParamsVS.Samplers[i].BindPoint] = cbParamsVS.Samplers[i].Name;
	}
	for (unsigned int i = 0; i < cbParamsVS.NumberTextures; ++i)
	{
		texBinds[cbParamsVS.Textures[i].BindPoint] = cbParamsVS.Textures[i].Name;
	}

	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsPS.NumberBuffers; ++i)
	{
		if(cbBinds.find(cbParamsPS.Buffers[i].BindPoint) == cbBinds.end())
			cbBinds[cbParamsPS.Buffers[i].BindPoint] = cbParamsPS.Buffers[i].Name;
	}
	for (unsigned int i = 0; i < cbParamsPS.NumberSamplers; ++i)
	{
		if (sampBinds.find(cbParamsPS.Samplers[i].BindPoint) == sampBinds.end())
			sampBinds[cbParamsPS.Samplers[i].BindPoint] = cbParamsPS.Samplers[i].Name;
	}
	for (unsigned int i = 0; i < cbParamsPS.NumberTextures; ++i)
	{
		if (texBinds.find(cbParamsPS.Textures[i].BindPoint) == texBinds.end())
			texBinds[cbParamsPS.Textures[i].BindPoint] = cbParamsPS.Textures[i].Name;
	}

	m_NumberSamplers = sampBinds.size();
	m_NumberTextures = texBinds.size();
	m_NumberConstantBuffers = cbBinds.size();

	m_pConstantBuffers = new IGpuBufferResource*[m_NumberConstantBuffers];
	m_pSamplers = new SamplerStateEntry[m_NumberSamplers];
	m_pTextures = new IGpuBufferResource*[m_NumberTextures];

	m_bIsInitialised = true;
}

void GpuResourceTable::Destroy(void)
{
	if (m_bIsInitialised)
	{
		ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
		for (unsigned int i = 0; i < cbParamsVS.NumberBuffers; ++i)
		{
			m_pConstantBuffers[i] = nullptr;
		}
		delete[] m_pConstantBuffers;
		m_pConstantBuffers = nullptr;
		m_pVertexShader = nullptr;

		ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
		delete[] m_pSamplers;

		for (unsigned int i = 0; i < cbParamsVS.NumberTextures; ++i)
		{
			m_pTextures[i] = nullptr;
		}
		delete[] m_pTextures;
		m_pTextures = nullptr;

		m_pPixelShader = nullptr;
	}
	m_bIsInitialised = false;
}

bool GpuResourceTable::SetTexture(const char* _pName, IGpuBufferResource* _pTexture)
{
	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsVS.NumberTextures; ++i)
	{
		if (strncmp(cbParamsVS.Textures[i].Name, _pName, ARRAYSIZE(cbParamsVS.Textures[i].Name)) == 0)
		{
			m_pTextures[cbParamsVS.Textures[i].BindPoint] = _pTexture;
			return true;
		}
	}

	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsPS.NumberTextures; ++i)
	{
		if (strncmp(cbParamsPS.Textures[i].Name, _pName, ARRAYSIZE(cbParamsPS.Textures[i].Name)) == 0)
		{
			m_pTextures[cbParamsPS.Textures[i].BindPoint] = _pTexture;
			return true;
		}
	}

	// Can ignore this CB Binding, Shaders aren't expecting it
	// assert(false && "SRV Resource Binding Failed");

	return false;
}

bool GpuResourceTable::SetConstantBuffer(const char* _pName, IGpuBufferResource* _pCBuffer)
{
	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsVS.NumberBuffers; ++i)
	{
		if (strncmp(cbParamsVS.Buffers[i].Name, _pName, ARRAYSIZE(cbParamsVS.Buffers[i].Name)) == 0)
		{
			m_pConstantBuffers[cbParamsVS.Buffers[i].BindPoint] = _pCBuffer;
			return true;
		}
	}
	
	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsPS.NumberBuffers; ++i)
	{
		if (strncmp(cbParamsPS.Buffers[i].Name, _pName, ARRAYSIZE(cbParamsPS.Buffers[i].Name)) == 0)
		{
			m_pConstantBuffers[cbParamsPS.Buffers[i].BindPoint] = _pCBuffer;
			return true;
		}
	}

	// Can ignore this CB Binding, Shaders aren't expecting it
	//assert(false && "CBV Resource Binding Failed");

	return false;
}

bool GpuResourceTable::SetSamplerState(const char* _pName, SamplerStateEntry _state)
{
	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsVS.NumberSamplers; ++i)
	{
		char targetName[32] = { 0 };
		snprintf(targetName, ARRAYSIZE(targetName), "%sSampler", _pName);
		if (strncmp(cbParamsVS.Samplers[i].Name, targetName, ARRAYSIZE(cbParamsVS.Samplers[i].Name)) == 0)
		{
			m_pSamplers[cbParamsVS.Samplers[i].BindPoint] = _state;
			return true;
		}
	}

	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
	for (unsigned int i = 0; i < cbParamsPS.NumberSamplers; ++i)
	{
		char targetName[32] = { 0 };
		snprintf(targetName, ARRAYSIZE(targetName), "%sSampler", _pName);
		if (strncmp(cbParamsPS.Samplers[i].Name, targetName, ARRAYSIZE(cbParamsPS.Samplers[i].Name)) == 0)
		{
			m_pSamplers[cbParamsPS.Samplers[i].BindPoint] = _state;
			return true;
		}
	}

	// Can ignore this CB Binding, Shaders aren't expecting it
	// assert(false && "Sampler State Binding Failed");

	return false;
}

unsigned long GpuResourceTable::GetTextures(IGpuBufferResource*** _pppResources)
{
	(*_pppResources) = m_pTextures;
	return m_NumberTextures;
}

unsigned long GpuResourceTable::GetConstantBuffers(IGpuBufferResource*** _pppResources)
{
	(*_pppResources) = m_pConstantBuffers;
	return m_NumberConstantBuffers;
}

unsigned long GpuResourceTable::GetSamplers(SamplerStateEntry** _ppResources)
{
	(*_ppResources) = m_pSamplers;
	return m_NumberSamplers;
}