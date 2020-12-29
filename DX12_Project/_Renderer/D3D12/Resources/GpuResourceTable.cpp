#include "GpuResourceTable.h"

#include "IShader.h"

#include "ConstantBufferParameters.h"
#include "ConstantBufferResource.h"

#include <assert.h>

GpuResourceTable::GpuResourceTable(void)
{
}

GpuResourceTable::GpuResourceTable(IShader* _pVS, IShader* _pPS)
{
	Initialise(_pVS, _pPS);
}

GpuResourceTable::~GpuResourceTable(void)
{
	Destroy();
}

void GpuResourceTable::Initialise(IShader* _pVS, IShader* _pPS)
{
	if (m_bIsInitialised)
		Destroy();

	m_pVertexShader = _pVS;
	m_pPixelShader = _pPS;

	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();
	m_pConstantBuffers = new IGpuBufferResource*[cbParamsVS.NumberBuffers];

	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();
	m_pSamplers = new SamplerStateEntry[cbParamsPS.NumberSamplers];
	m_pTextures = new IGpuBufferResource*[cbParamsPS.NumberTextures];

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
	assert(m_pPixelShader && "No Pixel Shader Set");

	ConstantBufferParameters cbParamsPS = m_pPixelShader->GetConstantParameters();

	for (unsigned int i = 0; i < cbParamsPS.NumberTextures; ++i)
	{
		if (strncmp(cbParamsPS.Textures[i].Name, _pName, ARRAYSIZE(cbParamsPS.Textures[i].Name)) == 0)
		{
			m_pTextures[cbParamsPS.Textures[i].BindPoint] = _pTexture;
			return true;
		}
	}
	assert(false && "SRV Resource Binding Failed");
	return false;
}

bool GpuResourceTable::SetConstantBuffer(const char* _pName, IGpuBufferResource* _pCBuffer)
{
	assert(m_pVertexShader && "No Vertex Shader Set");

	ConstantBufferParameters cbParamsVS = m_pVertexShader->GetConstantParameters();

	for (unsigned int i = 0; i < cbParamsVS.NumberBuffers; ++i)
	{
		if (strncmp(cbParamsVS.Buffers[i].Name, _pName, ARRAYSIZE(cbParamsVS.Buffers[i].Name)) == 0)
		{
			m_pConstantBuffers[cbParamsVS.Buffers[i].BindPoint] = _pCBuffer;
			return true;
		}
	}
	assert(false && "CBV Resource Binding Failed");
	return false;
}

bool GpuResourceTable::SetSamplerState(const char* _pName, SamplerStateEntry _state)
{
	assert(m_pPixelShader && "No Pixel Shader Set");

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
	assert(false && "Sampler State Binding Failed");
	return false;
}

unsigned long GpuResourceTable::GetTextures(IGpuBufferResource*** _pppResources)
{
	(*_pppResources) = m_pTextures;
	return m_pPixelShader->GetConstantParameters().NumberTextures;
}

unsigned long GpuResourceTable::GetConstantBuffers(IGpuBufferResource*** _pppResources)
{
	(*_pppResources) = m_pConstantBuffers;
	return m_pVertexShader->GetConstantParameters().NumberBuffers;
}

unsigned long GpuResourceTable::GetSamplers(SamplerStateEntry** _ppResources)
{
	(*_ppResources) = m_pSamplers;
	return m_pPixelShader->GetConstantParameters().NumberSamplers;
}