#include "Dx12Defines.h"

#include "Window/GameWindow.h"

#include "DeviceDx12.h"
#include "CommandListDx12.h"
#include "CommandQueueDx12.h"
#include "SwapChainDx12.h"
#include "DescriptorHeapDx12.h"

#include "Texture2DResourceDx12.h"
#include "VertexBufferResourceDx12.h"
#include "IndexBufferResourceDx12.h"
#include "ConstantBufferResourceDx12.h"
#include "DepthBufferResourceDx12.h"
#include "RenderTargetResourceDx12.h"

#include "Interfaces/IShader.h"
#include "Interfaces/IMaterial.h"

#include "SamplerStateDx12.h"
#include "ShaderDx12.h"

#include <assert.h>
#include <DirectXMath.h>

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#include "Hashing/Hashing.h"
#include "Helpers/ProfileMarker.h"

#include "Memory/ScopedMemoryRecord.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace Artemis::Core;
using namespace Artemis::Memory;
using namespace Artemis::Renderer::Interfaces;

namespace Artemis::Renderer::Device::Dx12
{
	using namespace Interfaces;

	void GenerateInputLayout( const Renderer::Interfaces::IShaderStage* _pShader, std::vector<D3D12_INPUT_ELEMENT_DESC>* _pLayout )
	{
		if ( _pShader->GetType() != Renderer::Interfaces::IShaderStage::ShaderType::EVertexShader )
		{
			//LogError( "Shader generating Input Layout IS NOT a Vertex Shader" );
			return;
		}

		const Renderer::Interfaces::ShaderIoParameters parameters = _pShader->GetShaderParameters();

		_pLayout->reserve( parameters.NumberInputs );

		for ( unsigned int input = 0; input < parameters.NumberInputs; ++input )
		{
			const Renderer::Interfaces::ShaderIoParameters::Parameter& p = parameters.Inputs[input];
			D3D12_INPUT_ELEMENT_DESC                                   desc;
			ZeroMemory( &desc, sizeof(D3D12_INPUT_ELEMENT_DESC) );
			desc.SemanticIndex        = p.SemanticIndex;
			desc.SemanticName         = p.SemanticName;
			desc.InputSlot            = 0;
			desc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;
			desc.AlignedByteOffset    = D3D12_APPEND_ALIGNED_ELEMENT;

			if ( p.Mask == 1 )
			{
				if ( p.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
					desc.Format = DXGI_FORMAT_R32_UINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
					desc.Format = DXGI_FORMAT_R32_SINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
					desc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if ( p.Mask <= 3 )
			{
				if ( p.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
					desc.Format = DXGI_FORMAT_R32G32_UINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
					desc.Format = DXGI_FORMAT_R32G32_SINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
					desc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if ( p.Mask <= 7 )
			{
				if ( p.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
					desc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
					desc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if ( p.Mask <= 15 )
			{
				if ( p.ComponentType == D3D_REGISTER_COMPONENT_UINT32 )
					desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_SINT32 )
					desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if ( p.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32 )
					desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			_pLayout->push_back( desc );
		}
	}

	DeviceDx12::DeviceDx12( void ) :
		m_DeviceState(),
		m_pDescHeapRtv( nullptr ),
		m_pDescHeapDsv( nullptr ),
		m_pDescHeapSrvCbv( nullptr ),
		m_pDescHeapSampler( nullptr ),
		m_pActiveResourceHeap( nullptr ),
		m_pActiveSamplerHeap( nullptr ),
		m_pDefaultSampler( nullptr ),
		m_eDefaultSampler(),
		m_pImmediateContext( nullptr )
	{
		m_pDevice      = nullptr;
		m_pDxgiFactory = nullptr;
		m_pDxgiAdapter = nullptr;

		Initialise( false );
	}

	DeviceDx12::~DeviceDx12( void )
	{
		if ( m_pDevice )
			m_pDevice.Reset();
		if ( m_pDxgiFactory )
			m_pDxgiFactory.Reset();
		if ( m_pDxgiAdapter )
			m_pDxgiAdapter.Reset();
	}

	bool DeviceDx12::Initialise( bool _bDebugging )
	{
		// Query Adapter
		HRESULT hr = S_OK;
		{
			hr = CreateDXGIFactory2( DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS( m_pDxgiFactory.GetAddressOf() ) );
			if ( FAILED( hr ) )
			{
				assert( false && "DXGI Factory Creation Failed" );
				return false;
			}

			SIZE_T                maxDedicatedVMEM = 0;
			ComPtr<IDXGIAdapter1> pAdapter1        = nullptr;
			for ( UINT adapter = 0; m_pDxgiFactory->EnumAdapters1( adapter, pAdapter1.GetAddressOf() ) != DXGI_ERROR_NOT_FOUND; ++adapter )
			{
				DXGI_ADAPTER_DESC1 adapterDesc1 = {};
				ZeroMemory( &adapterDesc1, sizeof(DXGI_ADAPTER_DESC1) );
				pAdapter1->GetDesc1( &adapterDesc1 );

				const bool bIsSoftwareFlag  = (adapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
				const bool bCanCreateDevice = SUCCEEDED( D3D12CreateDevice(pAdapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr) );
				const bool bHasMoreVmem     = adapterDesc1.DedicatedVideoMemory > maxDedicatedVMEM;
				if ( bIsSoftwareFlag && bCanCreateDevice && bHasMoreVmem )
				{
					maxDedicatedVMEM = adapterDesc1.DedicatedVideoMemory;
					hr               = pAdapter1.As( &m_pDxgiAdapter );
					if ( FAILED( hr ) )
					{
						assert( false && "Found appropriate adapter but failed casting to IDXGIAdapter4" );
						return false;
					}
				}
			}
		}

		// If Debugging Enabled
		if ( _bDebugging )
		{
			ComPtr<ID3D12Debug>  spDebugController0;
			ComPtr<ID3D12Debug1> spDebugController1;
			hr = D3D12GetDebugInterface( IID_PPV_ARGS( &spDebugController0 ) );
			if ( FAILED( hr ) )
			{
				assert( false && "No Debug Interface" );
			}

			hr = spDebugController0->QueryInterface( IID_PPV_ARGS( &spDebugController1 ) );
			if ( FAILED( hr ) )
			{
				assert( false && "No Debug 1 Interface" );
			}

			spDebugController1->EnableDebugLayer();

			ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings = nullptr;
			hr                                                            = D3D12GetDebugInterface( IID_PPV_ARGS( &pDredSettings ) );
			if ( SUCCEEDED( hr ) )
			{
				pDredSettings->SetAutoBreadcrumbsEnablement( D3D12_DRED_ENABLEMENT_FORCED_ON );
				pDredSettings->SetPageFaultEnablement( D3D12_DRED_ENABLEMENT_FORCED_ON );
			}
			spDebugController1->SetEnableGPUBasedValidation( true );
		}

		// --------------------------------------------------------------------------------------------------------------------------------
		// Experimental Feature: D3D12ExperimentalShaderModels
		//
		// Use with D3D12EnableExperimentalFeatures to enable experimental shader model support,
		// meaning shader models that haven't been finalized for use in retail.
		//
		// Enabling D3D12ExperimentalShaderModels needs no configuration struct, pass NULL in the pConfigurationStructs array.
		//
		// --------------------------------------------------------------------------------------------------------------------------------
		static constexpr UUID kD3D12ExperimentalShaderModels = {
			/* 76f5573e-f13a-40f5-b297-81ce9e18933f */
			0x76f5573e, 0xf13a, 0x40f5, {0xb2, 0x97, 0x81, 0xce, 0x9e, 0x18, 0x93, 0x3f}
		};

		VALIDATE_D3D( D3D12EnableExperimentalFeatures( 1, &kD3D12ExperimentalShaderModels, nullptr, nullptr ) );

		// Create Device
		VALIDATE_D3D( D3D12CreateDevice( m_pDxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS( m_pDevice.GetAddressOf() ) ) );
		m_pDevice->SetName( L"Le Device" );

		if ( _bDebugging )
		{
			ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
			if ( SUCCEEDED( m_pDevice.As(&pInfoQueue) ) )
			{
				pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, TRUE );
				pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, FALSE );
				pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
			}
		}

		constexpr unsigned int kMaxSrvCbvs = 1000;
		if ( !CreateDescriptorHeap( DescriptorHeapType_CbvSrvUav, &m_pDescHeapSrvCbv, DescriptorHeapFlags_None, kMaxSrvCbvs, L"MainSrvCbvHeap" ) )
			return false;

		constexpr unsigned int kMaxSamplerDescs = 1;
		if ( !CreateDescriptorHeap( DescriptorHeapType_Sampler, &m_pDescHeapSampler, DescriptorHeapFlags_None, kMaxSamplerDescs ) )
			return false;

		// Default Sampler
		m_pDefaultSampler = CreateSamplerState( Renderer::Interfaces::SamplerStateFilter::ELinear, Renderer::Interfaces::SamplerStateWrapMode::EWrap, Renderer::Interfaces::SamplerStateComparisonFunction::EAlways );

		m_pImmediateContext = new CommandListDx12();
		if ( !CreateCommandList( CommandListType_Direct, &m_pImmediateContext, L"ImmediateContext" ) )
		{
			assert( "Command List Creation Failed" );
		}

		return true;
	}

	bool DeviceDx12::InitialiseImGui( Artemis::Core::GameWindow* _pWindow, const Interfaces::IDescriptorHeap* _pSrvHeap ) const
	{
		ImGui_ImplWin32_Init( _pWindow->GetWindowHandle() );
		ID3D12DescriptorHeap* pHeap = static_cast<ID3D12DescriptorHeap*>(_pSrvHeap->GetDeviceObject());
		ImGui_ImplDX12_Init( m_pDevice.Get(), 1, DXGI_FORMAT_R8G8B8A8_UNORM, pHeap, pHeap->GetCPUDescriptorHandleForHeapStart(), pHeap->GetGPUDescriptorHandleForHeapStart() );
		return true;
	}

	bool DeviceDx12::CreateCommandQueue( Interfaces::ECommandListType _type, Interfaces::ICommandQueue** _ppCommandQueue, const wchar_t* _pDebugName ) const
	{
		const IGraphicsDevice* pThis = this;
		if ( !(*_ppCommandQueue)->Initialise( this, CommandListType_Direct, _pDebugName ) )
			return false;

		return true;
	}

	bool DeviceDx12::CreateCommandList( Interfaces::ECommandListType _type, ICommandList** _ppCommandList, const wchar_t* _pDebugName ) const
	{
		*_ppCommandList = new CommandListDx12();
		if ( !(*_ppCommandList)->Initialise( this, CommandListType_Direct, _pDebugName ) )
			return false;

		return true;
	}

	bool DeviceDx12::CreateDescriptorHeap( const DescriptorHeapType _type, IDescriptorHeap** _pDescriptorHeap, const DescriptorHeapFlags _flags, const unsigned int _numBuffers, const wchar_t* _pDebugName ) const
	{
		if ( !(*_pDescriptorHeap) )
		{
			*_pDescriptorHeap = new DescriptorHeapDx12();
		}

		if ( !(*_pDescriptorHeap)->Initialise( this, _type, _numBuffers, _flags, _pDebugName ) )
			return false;

		return true;
	}

	bool DeviceDx12::CreateSwapChain( Interfaces::ISwapChain** _ppSwapChain, ICommandQueue* _pCmdQueue, Core::GameWindow* _pWindow, unsigned int _numBackBuffers, const wchar_t* _pDebugName )
	{
		if ( !CreateDescriptorHeap( DescriptorHeapType_Rtv, &m_pDescHeapRtv, DescriptorHeapFlags_None, _numBackBuffers, _pDebugName ) )
			return false;

		if ( !CreateDescriptorHeap( DescriptorHeapType_Dsv, &m_pDescHeapDsv, DescriptorHeapFlags_None, 1, _pDebugName ) )
			return false;

		*_ppSwapChain = new SwapChainDx12( m_pDxgiFactory.Get() );
		if ( !(*_ppSwapChain)->Initialise( this, _pCmdQueue, _numBackBuffers, m_pDescHeapRtv, m_pDescHeapDsv, _pWindow ) )
			return false;

		return true;
	}

	Renderer::Interfaces::ISamplerState* DeviceDx12::CreateSamplerState( const Renderer::Interfaces::SamplerStateFilter _eFilter, const Renderer::Interfaces::SamplerStateWrapMode _eWrap, const Renderer::Interfaces::SamplerStateComparisonFunction _eCompFunc ) const
	{
		SamplerStateDx12* pSamplerState = new SamplerStateDx12();
		if ( !pSamplerState->Initialise( this, m_pDescHeapSampler, _eFilter, _eWrap, _eCompFunc ) )
		{
			delete pSamplerState;
			return nullptr;
		}
		return pSamplerState;
	}

#pragma region
	IGpuResource* DeviceDx12::CreateTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName ) const
	{
		return new Texture2DResourceDx12( _pWstrFilename, true, this, _pCommandList, m_pDescHeapSrvCbv, _pDebugName );
	}

	IGpuResource* DeviceDx12::CreateWicTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName ) const
	{
		return new Texture2DResourceDx12( _pWstrFilename, false, this, _pCommandList, m_pDescHeapSrvCbv, _pDebugName );
	}

	IGpuResource* DeviceDx12::CreateVertexBufferResource( IACommandList* _pCommandList, const unsigned int _sizeInBytes, const unsigned int _strideInBytes, const EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName ) const
	{
		return new VertexBufferResourceDx12( this, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName );
	}

	IGpuResource* DeviceDx12::CreateIndexBufferResource( IACommandList* _pCommandList, const unsigned int _sizeInBytes, const unsigned int _strideInBytes, EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName ) const
	{
		return new IndexBufferResourceDx12( this, _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName );
	}

	IGpuResource* DeviceDx12::CreateConstantBufferResource( const Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName ) const
	{
		return new ConstantBufferResourceDx12( this, m_pDescHeapSrvCbv, _params, _pDebugName );
	}

	IGpuResource* DeviceDx12::CreateRenderTargetResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pRtvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const
	{
		return new RenderTargetResourceDx12( _pDevice, _pCmdList, _width, _height, _format, _pRtvHeap, m_pDescHeapSrvCbv );
	}

	IGpuResource* DeviceDx12::CreateDepthBufferResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pDsvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const
	{
		return new DepthBufferResourceDx12( _pDevice, _pCmdList, _width, _height, _format, _pDsvHeap, m_pDescHeapSrvCbv );
	}

	bool DeviceDx12::GetRootSignature( IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName )
	{
		if ( const unsigned long ulHash = SimpleHash( static_cast<const char*>(m_DeviceState.GetShader( Interfaces::IShaderStage::ShaderType::EVertexShader )->GetShaderName()), strlen( m_DeviceState.GetShader( Interfaces::IShaderStage::ShaderType::EVertexShader )->GetShaderName() ) ); m_mapRootSignatures.contains( ulHash ) )
		{
			(*_ppRootSignature) = m_mapRootSignatures[ulHash];
		}
		else
		{
			Helpers::RenderMarker profile( GetImmediateContext(), "ID3D12Device::CreateRootSignature" );

			ISamplerState**     pSamplers  = nullptr;
			IGpuResource**      ppBuffers  = nullptr,**ppTextures = nullptr;
			const unsigned long ulSamplers = m_DeviceState.m_uiNumberSamplers;
			const unsigned long ulTextures = m_DeviceState.m_uiNumberTextures;
			const unsigned long ulCBuffers = m_DeviceState.m_uiNumberConstantBuffers;

			CD3DX12_DESCRIPTOR_RANGE1 table1[2];
			table1[0].Init( D3D12_DESCRIPTOR_RANGE_TYPE_CBV, ulCBuffers, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE );
			table1[1].Init( D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ulTextures, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE );

			CD3DX12_DESCRIPTOR_RANGE1 table2;
			table2.Init( D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, ulSamplers, 0 );

			CD3DX12_ROOT_PARAMETER1 rootParameters[2];
			rootParameters[0].InitAsDescriptorTable( ARRAYSIZE( table1 ), table1, D3D12_SHADER_VISIBILITY_ALL );
			rootParameters[1].InitAsDescriptorTable( 1, &table2, D3D12_SHADER_VISIBILITY_ALL );

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig;
			rootSig.Init_1_1( ARRAYSIZE( rootParameters ), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS );

			ID3DBlob* pSerializedRootSig;
			VALIDATE_D3D( D3D12SerializeVersionedRootSignature( &rootSig, &pSerializedRootSig, nullptr ) );

			ID3D12RootSignature* pRootSig = nullptr;
			VALIDATE_D3D( m_pDevice->CreateRootSignature( 0, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS( &pRootSig ) ) );
			pRootSig->SetName( _pDebugName );

			m_mapRootSignatures[ulHash] = pRootSig;
			(*_ppRootSignature)         = pRootSig;
		}

		return true;
	}

	bool DeviceDx12::GetPipelineState( ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName )
	{
		ID3D12RootSignature* pRootSignature = nullptr;
		if ( !GetRootSignature( m_DeviceState.m_pVertexShader, &pRootSignature ) )
		{
			LogError( "Root Signature Creation Failed." );
			return false;
		}

		m_DeviceState.m_stats.RootSignatureUpdates++;
		const CommandListDx12* pImmediateContext = static_cast<CommandListDx12*>(GetImmediateContext());
		pImmediateContext->SetGraphicsRootSignature( pRootSignature );

		D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc;
		ZeroMemory( &stateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC) );

		// Default
		{
			stateDesc.RasterizerState   = m_DeviceState.RasterizerState;
			stateDesc.BlendState        = m_DeviceState.BlendState;
			stateDesc.DepthStencilState = m_DeviceState.DepthStencilState;

			stateDesc.DSVFormat             = DXGI_FORMAT_D32_FLOAT;
			stateDesc.SampleMask            = UINT_MAX;
			stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			PRAGMA_TODO( "Change Formats on Changed RTV/DSV" );
			stateDesc.NumRenderTargets = 1;
			stateDesc.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;
			stateDesc.SampleDesc.Count = 1;
		}

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		GenerateInputLayout( m_DeviceState.m_pVertexShader, &inputLayout );

		unsigned long ulHash = 0;
		// Parameterized
		{
			LOW_LEVEL_PROFILE_MARKER( GetImmediateContext(), "GetPipelineState::Hash Ident" );

			stateDesc.InputLayout    = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};
			stateDesc.pRootSignature = pRootSignature;
			stateDesc.VS             = {m_DeviceState.m_pVertexShader->GetBytecode(), m_DeviceState.m_pVertexShader->GetBytecodeSize()};
			stateDesc.PS             = {m_DeviceState.m_pPixelShader->GetBytecode(), m_DeviceState.m_pPixelShader->GetBytecodeSize()};

			ulHash += SimpleHash( reinterpret_cast<const char*>(&inputLayout[0]), sizeof( D3D12_INPUT_ELEMENT_DESC ) * inputLayout.size() );
			ulHash += SimpleHash( reinterpret_cast<const char*>(&stateDesc.pRootSignature), sizeof( ID3D12RootSignature ) );
			ulHash += SimpleHash( m_DeviceState.m_pVertexShader->GetShaderName(), strlen( m_DeviceState.m_pVertexShader->GetShaderName() ) );
			ulHash += SimpleHash( m_DeviceState.m_pPixelShader->GetShaderName(), strlen( m_DeviceState.m_pPixelShader->GetShaderName() ) );
			ulHash += SimpleHash( reinterpret_cast<const char*>(&stateDesc.RasterizerState), sizeof( D3D12_RASTERIZER_DESC ) );
			ulHash += SimpleHash( reinterpret_cast<const char*>(&stateDesc.BlendState), sizeof( D3D12_BLEND_DESC ) );
			ulHash += SimpleHash( reinterpret_cast<const char*>(&stateDesc.DepthStencilState), sizeof( D3D12_DEPTH_STENCIL_DESC ) );
		}

		if ( m_mapPso.find( ulHash ) != m_mapPso.end() )
		{
			(*_ppPipelineState) = m_mapPso[ulHash];
		}
		else
		{
			LOW_LEVEL_PROFILE_MARKER( GetImmediateContext(), "ID3D12Device::CreatePipelineState" );

			ID3D12PipelineState* pPso = nullptr;
			VALIDATE_D3D( m_pDevice->CreateGraphicsPipelineState( &stateDesc, IID_PPV_ARGS( &pPso ) ) );
			pPso->SetName( _pDebugName );

			m_mapPso[ulHash]    = pPso;
			(*_ppPipelineState) = pPso;
		}

		return true;
	}

	bool DeviceDx12::CreateSamplerState( const D3D12_SAMPLER_DESC* _pSamplerDesc, const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName ) const
	{
		m_pDevice->CreateSampler( _pSamplerDesc, _cpuHandle );
		return true;
	}

	void DeviceDx12::BeginFrame( void )
	{
		m_pImmediateContext->Reset();

		m_DeviceState.SetDirty( DeviceState::kDirtyPipelineState );

		constexpr unsigned int kElements = 2000;
		if ( !CreateDescriptorHeap( DescriptorHeapType_CbvSrvUav, &m_pActiveResourceHeap, DescriptorHeapFlags_ShaderVisible, kElements, L"Temp Texture Heap" ) )
		{
			assert( false && "Temp Texture Heap Creation Failure" );
		}

		if ( !CreateDescriptorHeap( DescriptorHeapType_Sampler, &m_pActiveSamplerHeap, DescriptorHeapFlags_ShaderVisible, kElements, L"Temp Sampler Heap" ) )
		{
			assert( false && "Temp Texture Heap Creation Failure" );
		}
	}

	void DeviceDx12::EndFrame( void )
	{
		m_DeviceState.m_stats.ConstantBufferUpdates = 0;
		m_DeviceState.m_stats.SamplerStateUpdates   = 0;
		m_DeviceState.m_stats.TextureUpdates        = 0;
		m_DeviceState.m_stats.RenderTargetUpdates   = 0;
		m_DeviceState.m_stats.DepthBufferUpdates    = 0;
		m_DeviceState.m_stats.RootSignatureUpdates  = 0;
		m_DeviceState.m_stats.PipelineStateUpdates  = 0;
		m_DeviceState.m_stats.ShaderUpdates         = 0;

		m_pActiveResourceHeap->Reset();
		m_pActiveSamplerHeap->Reset();
	}

	bool DeviceDx12::FlushState()
	{
		const CommandListDx12* pGfxCmdList = static_cast<CommandListDx12*>(GetImmediateContext());

		LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "FlushState" );

		if ( m_DeviceState.IsDirty( DeviceState::kDirtyPipelineState ) )
		{
			ID3D12PipelineState* pPso = nullptr;
			if ( !GetPipelineState( &pPso ) )
				return false;

			pGfxCmdList->SetPipelineState( pPso );
			m_DeviceState.m_stats.PipelineStateUpdates++;
		}

		ID3D12DescriptorHeap* pDescHeapRtv      = static_cast<ID3D12DescriptorHeap*>(m_pActiveResourceHeap->GetDeviceObject());
		ID3D12DescriptorHeap* pDescHeapSamplers = static_cast<ID3D12DescriptorHeap*>(m_pActiveSamplerHeap->GetDeviceObject());

		IDescriptorHeap* pHeaps[] = {m_pActiveResourceHeap, m_pActiveSamplerHeap};
		pGfxCmdList->SetDescriptorHeaps( pHeaps, _countof( pHeaps ) );

		const unsigned int uiResourceStartIndex = m_pActiveResourceHeap->GetFreeIndex();

		//
		// Copy CBVs
		//
		GpuResourceDx12** ppCBs       = nullptr;
		unsigned long     ulResources = m_DeviceState.GetConstantBuffers( &ppCBs );
		for ( unsigned int i = 0; i < ulResources; ++i )
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource( pDescHeapRtv->GetCPUDescriptorHandleForHeapStart(), m_pActiveResourceHeap->GetFreeIndexAndIncrement(), m_pActiveResourceHeap->GetIncrementSize() );

			if ( !ppCBs[i] )
			{
				LogWarning( "Null Constant Buffer" );
				continue;
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppCBs[i]->GetCbvCpuHandle();

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "CBV Desc Copies" );
			m_pDevice->CopyDescriptors( 1, &hCpuNewResource, &size1, 1, &hCpuActual, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		}

		// 
		// Copy SRVs
		//
		GpuResourceDx12** ppTextures = nullptr;;
		ulResources                  = m_DeviceState.GetTextures( &ppTextures );
		for ( unsigned int i = 0; i < ulResources; ++i )
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource( pDescHeapRtv->GetCPUDescriptorHandleForHeapStart(), m_pActiveResourceHeap->GetFreeIndexAndIncrement(), m_pActiveResourceHeap->GetIncrementSize() );

			if ( !ppTextures[i] )
			{
				//LogWarning_Renderer("Invalid Texture");

				static D3D12_SHADER_RESOURCE_VIEW_DESC staticNullDesc;
				staticNullDesc.Format                        = DXGI_FORMAT_R8G8B8A8_UNORM;
				staticNullDesc.ViewDimension                 = D3D12_SRV_DIMENSION_TEXTURE2D;
				staticNullDesc.Texture2D.MipLevels           = 0;
				staticNullDesc.Texture2D.MostDetailedMip     = 0;
				staticNullDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				staticNullDesc.Shader4ComponentMapping       = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				m_pDevice->CreateShaderResourceView( nullptr, &staticNullDesc, hCpuNewResource );

				continue;
			}

			CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppTextures[i]->GetSrvCpuHandle();

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "SRV Desc Copies" );
			m_pDevice->CopyDescriptors( 1, &hCpuNewResource, &size1, 1, &hCpuActual, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
		}

		//
		// Copy Sampler
		//			
		SamplerStateDx12** pSamplers           = nullptr;
		ulResources                            = m_DeviceState.GetSamplers( &pSamplers );
		const unsigned int uiSamplerStartIndex = m_pActiveSamplerHeap->GetFreeIndex();
		for ( unsigned int i = 0; i < ulResources; ++i )
		{
			ID3D12DescriptorHeap* pSamplerHeap = static_cast<ID3D12DescriptorHeap*>(m_pDescHeapSampler->GetDeviceObject());

			CD3DX12_CPU_DESCRIPTOR_HANDLE sampDescHandle( pSamplerHeap->GetCPUDescriptorHandleForHeapStart() );
			sampDescHandle.Offset( static_cast<SamplerStateDx12*>(pSamplers[i])->GetHeapIndex(), m_pDescHeapSampler->GetIncrementSize() );

			CD3DX12_CPU_DESCRIPTOR_HANDLE tempHandleLoc( pDescHeapSamplers->GetCPUDescriptorHandleForHeapStart(), m_pActiveSamplerHeap->GetFreeIndexAndIncrement(), m_pActiveSamplerHeap->GetIncrementSize() );

			UINT size1 = 1;
			UINT size2 = 1;

			LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "Sampler Desc Copies" );
			m_pDevice->CopyDescriptors( 1, &tempHandleLoc, &size1, 1, &sampDescHandle, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
		}

		pGfxCmdList->SetGraphicsRootDescriptorTable( 0, CD3DX12_GPU_DESCRIPTOR_HANDLE( pDescHeapRtv->GetGPUDescriptorHandleForHeapStart(), uiResourceStartIndex, m_pActiveResourceHeap->GetIncrementSize() ) );
		pGfxCmdList->SetGraphicsRootDescriptorTable( 1, CD3DX12_GPU_DESCRIPTOR_HANDLE( pDescHeapSamplers->GetGPUDescriptorHandleForHeapStart(), uiSamplerStartIndex, m_pActiveSamplerHeap->GetIncrementSize() ) );

		m_DeviceState.DirtyFlags = 0;

		return true;
	}

	bool DeviceDx12::SetMaterial( Interfaces::IMaterial* _pMaterial )
	{
		if ( _pMaterial->m_pVertexShader != m_DeviceState.GetShader( Interfaces::IShaderStage::ShaderType::EVertexShader ) || _pMaterial->m_pPixelShader != m_DeviceState.GetShader( Interfaces::IShaderStage::ShaderType::EPixelShader ) )
		{
			m_DeviceState.Reinitialise( _pMaterial->m_pVertexShader, _pMaterial->m_pPixelShader );
		}

		return true;
	}

	bool DeviceDx12::SetRenderTarget( void )
	{
		return true;
	}

	bool DeviceDx12::SetDepthBuffer( void )
	{
		return true;
	}

	bool DeviceDx12::SetTexture( const char* _pName, IGpuResource* _pTexture )
	{
		if ( m_DeviceState.GetTexture( _pName ) != _pTexture )
		{
			return m_DeviceState.SetTexture( _pName, _pTexture );
		}

		return true;
	}

	bool DeviceDx12::SetConstantBuffer( const char* _pName, IGpuResource* _pCBuffer )
	{
		if ( m_DeviceState.GetConstantBuffer( _pName ) != _pCBuffer )
		{
			return m_DeviceState.SetConstantBuffer( _pName, _pCBuffer );
		}

		return true;
	}

	bool DeviceDx12::SetSamplerState( const char* _pName, ISamplerState* _pSamplerState )
	{
		if ( m_DeviceState.GetSampler( _pName ) != _pSamplerState )
		{
			return m_DeviceState.SetSampler( _pName, _pSamplerState );
		}

		return true;
	}

	bool DeviceDx12::SetRasterizerState( const Interfaces::RasteriserStateDesc& _desc )
	{
		m_DeviceState.SetRasterizerState( _desc );
		return true;
	}

	bool DeviceDx12::SetBlendState( const Interfaces::BlendDesc& _desc )
	{
		m_DeviceState.SetBlendState( _desc );
		return true;
	}

	bool DeviceDx12::SetDepthStencilState( const Interfaces::DepthStencilDesc& _desc )
	{
		m_DeviceState.SetDepthStencilState( _desc );
		return true;
	}
}
