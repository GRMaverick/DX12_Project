#include "GpuResourceDx12.h"
#include "Interfaces/ICommandList.h"

#include <cassert>

#include "Memory/MemoryGlobalTracking.h"

namespace Artemis::Renderer::Device::Dx12
{
	GpuResourceDx12::~GpuResourceDx12( void )
	{
		if ( m_cpuBuffer )
		{
			Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_cpuBuffer );
			m_cpuBuffer->Release();
			delete m_cpuBuffer;
		}

		if ( m_gpuBuffer )
		{
			Artemis::Memory::MemoryGlobalTracking::RecordExplicitDellocation( m_gpuBuffer );
			m_gpuBuffer->Release();
			delete m_gpuBuffer;
		}
	}

	bool GpuResourceDx12::UploadResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, Interfaces::ResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName )
	{
		HRESULT hr = S_OK;

		const UINT bufferSize = _sizeInBytes;

		const D3D12_RESOURCE_DESC   dRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize, static_cast<D3D12_RESOURCE_FLAGS>(_flags) );
		const D3D12_HEAP_PROPERTIES dHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );

		const ID3D12Device* pConstDevice = static_cast<const ID3D12Device*>(_pDevice->GetDeviceObject());
		ID3D12Device*       pDevice      = const_cast<ID3D12Device*>(pConstDevice);

		hr = pDevice->CreateCommittedResource( &dHeapProperties, D3D12_HEAP_FLAG_NONE, &dRdBuffer, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_gpuBuffer ) );
		if ( FAILED( hr ) )
		{
			assert( false && "Destination Buffer Setup Failed" );
			return false;
		}

		Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::EGeometryCpu, m_cpuBuffer, bufferSize );

		if ( _pData )
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> pIntermediate = nullptr;

			const D3D12_HEAP_PROPERTIES iHeapProperties = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
			const D3D12_RESOURCE_DESC   iRdBuffer       = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );

			hr = pDevice->CreateCommittedResource( &iHeapProperties, D3D12_HEAP_FLAG_NONE, &iRdBuffer, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS( &m_cpuBuffer ) );
			if ( FAILED( hr ) )
			{
				assert( false && "Upload Buffer Setup Failed" );
				return false;
			}

			Artemis::Memory::MemoryGlobalTracking::RecordExplicitAllocation( Artemis::Memory::MemoryContextCategory::EGeometryGpu, m_cpuBuffer, bufferSize );

			D3D12_SUBRESOURCE_DATA srData = {};
			ZeroMemory( &srData, sizeof(D3D12_SUBRESOURCE_DATA) );
			srData.pData      = _pData;
			srData.RowPitch   = bufferSize;
			srData.SlicePitch = srData.RowPitch;

			_pCommandList->UpdateSubresource( m_gpuBuffer, m_cpuBuffer, 0, 0, 1, &srData );
		}

		return true;
	}

	void GpuResourceDx12::BuildShaderResourceView( ID3D12Device* _pDevice, Interfaces::IDescriptorHeap* _pSrvHeap )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format                          = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip       = 0;
		srvDesc.Texture2D.MipLevels             = 1;
		srvDesc.Texture2D.ResourceMinLODClamp   = 0.0f;
		srvDesc.Texture2D.PlaneSlice            = 0;

		m_SrvHeapIndex = _pSrvHeap->GetFreeIndexAndIncrement();

		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pSrvHeap->GetDeviceObject());
		m_hSrvCpuHandle             = CD3DX12_CPU_DESCRIPTOR_HANDLE( pHeap->GetCPUDescriptorHandleForHeapStart() );
		m_hSrvCpuHandle.Offset( m_SrvHeapIndex, _pSrvHeap->GetIncrementSize() );

		_pDevice->CreateShaderResourceView( m_cpuBuffer, &srvDesc, m_hSrvCpuHandle );
	}

	unsigned long GpuResourceDx12::GetMemoryRequirement( unsigned _width, unsigned _height, unsigned _depth, DXGI_FORMAT _format )
	{
		unsigned long bitsPerPixel = 0;
		switch ( _format )
		{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			case DXGI_FORMAT_R32G32B32A32_UINT:
				bitsPerPixel = 128;
				break;

			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_SINT:
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			case DXGI_FORMAT_R32G32B32_UINT:
				bitsPerPixel = 96;
				break;

			case DXGI_FORMAT_R32G8X24_TYPELESS:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_TYPELESS:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
				bitsPerPixel = 64;
				break;

			case DXGI_FORMAT_G8R8_G8B8_UNORM:
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			case DXGI_FORMAT_R10G10B10A2_UINT:
			case DXGI_FORMAT_R10G10B10A2_UNORM:
			case DXGI_FORMAT_R11G11B10_FLOAT:
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R16G16_TYPELESS:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
				bitsPerPixel = 32;
				break;

			case DXGI_FORMAT_R8G8_TYPELESS:
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_D16_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_SINT:
				bitsPerPixel = 16;
				break;

			case DXGI_FORMAT_R8_TYPELESS:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_A8_UNORM:
				bitsPerPixel = 8;
				break;

			case DXGI_FORMAT_R1_UNORM:
				bitsPerPixel = 1;
				break;

			case DXGI_FORMAT_BC1_TYPELESS:
			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				bitsPerPixel = 17;

			case DXGI_FORMAT_420_OPAQUE:
			case DXGI_FORMAT_UNKNOWN: default:
				break; // assert(false && "Unrecognised Format");
		}
		return _width * _height * _depth * bitsPerPixel;
	}

	void GpuResourceDx12::SetCpuBuffer( ID3D12Resource* _pBuffer )
	{
		m_cpuBuffer = _pBuffer;
	}

	void GpuResourceDx12::SetGpuBuffer( ID3D12Resource* _pBuffer )
	{
		m_gpuBuffer = _pBuffer;
	}

	ID3D12Resource* GpuResourceDx12::GetCpuBuffer( void ) const
	{
		return m_cpuBuffer;
	}

	ID3D12Resource* GpuResourceDx12::GetGpuBuffer( void ) const
	{
		return m_gpuBuffer;
	}
}
