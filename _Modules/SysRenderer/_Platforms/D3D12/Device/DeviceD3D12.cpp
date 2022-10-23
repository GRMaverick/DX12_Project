#include "Defines.h"

#include "CoreWindow.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"
#include "D3D12\Device\SwapChain.h"

#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\Texture2DResource.h"
#include "D3D12\Resources\VertexBufferResource.h"
#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\ConstantBufferResource.h"
#include "D3D12\Resources\UploadBuffer.h"
#include "D3D12\Resources\GpuResourceTable.h"

#include "D3D12\States\SamplerStateD3D12.h"

#include "D3D12\Shaders\ShaderCache.h"

#include <assert.h>
#include <DirectXMath.h>

#include <ImGUI\imgui_impl_win32.h>
#include <ImGUI\imgui_impl_dx12.h>

#include "TextureLoader.h"
#include <WICTextureLoader.h>

#include "SysUtilities\Hashing.h"
#include "SysUtilities\_Profiling\ProfileMarker.h"

#include "SysMemory/include/ScopedMemoryRecord.h"

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysCore;
using namespace SysMemory;
using namespace SysUtilities;

namespace SysRenderer
{
	using namespace Interfaces;

	namespace D3D12
	{
		void GenerateInputLayout( const IShaderStage* _pShader, std::vector<D3D12_INPUT_ELEMENT_DESC>* _pLayout )
		{
			if ( _pShader->GetType() != IShaderStage::ShaderType::EVertexShader )
			{
				LogError( "Shader generating Input Layout IS NOT a Vertex Shader" );
				return;
			}

			const ShaderIoParameters parameters = _pShader->GetShaderParameters();

			_pLayout->reserve( parameters.NumberInputs );

			for ( unsigned int input = 0; input < parameters.NumberInputs; ++input )
			{
				const ShaderIoParameters::Parameter& p = parameters.Inputs[input];
				D3D12_INPUT_ELEMENT_DESC             desc;
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

		DeviceD3D12* DeviceD3D12::Instance( void )
		{
			static DeviceD3D12 device;
			return &device;
		}

		DeviceD3D12::DeviceD3D12( void ) :
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
		}

		DeviceD3D12::~DeviceD3D12( void )
		{
			if ( m_pDevice )
				m_pDevice.Reset();
			if ( m_pDxgiFactory )
				m_pDxgiFactory.Reset();
			if ( m_pDxgiAdapter )
				m_pDxgiAdapter.Reset();
		}

		bool DeviceD3D12::Initialise( bool _bDebugging )
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

			VALIDATE_D3D( D3D12EnableExperimentalFeatures(1, &kD3D12ExperimentalShaderModels, nullptr, nullptr) );

			// Create Device
			VALIDATE_D3D( D3D12CreateDevice(m_pDxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m_pDevice.GetAddressOf())) );
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
			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pDescHeapSrvCbv, kMaxSrvCbvs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, L"MainSrvCbvHeap" ) )
				return false;

			constexpr unsigned int kMaxSamplerDescs = 1;
			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_pDescHeapSampler, kMaxSamplerDescs, D3D12_DESCRIPTOR_HEAP_FLAG_NONE ) )
				return false;

			// Default Sampler
			m_pDefaultSampler = CreateSamplerState( SamplerStateFilter::ELinear, SamplerStateWrapMode::EWrap, SamplerStateComparisonFunction::EAlways );

			return true;
		}

		bool DeviceD3D12::InitialiseImGui( const HWND _hWindow, const DescriptorHeap* _pSrvHeap ) const
		{
			ImGui_ImplWin32_Init( _hWindow );
			ImGui_ImplDX12_Init( m_pDevice.Get(), 1, DXGI_FORMAT_R8G8B8A8_UNORM, _pSrvHeap->m_pDescriptorHeap.Get(), _pSrvHeap->GetCpuStartHandle(), _pSrvHeap->GetGpuStartHandle() );
			return true;
		}

		bool DeviceD3D12::CreateCommandQueue( D3D12_COMMAND_LIST_TYPE _type, CommandQueue** _ppCommandQueue, const wchar_t* _pDebugName ) const
		{
			if ( !(*_ppCommandQueue)->Initialise( m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, _pDebugName ) )
				return false;

			return true;
		}

		bool DeviceD3D12::CreateCommandList( D3D12_COMMAND_LIST_TYPE _type, CommandList** _ppCommandList, const wchar_t* _pDebugName ) const
		{
			*_ppCommandList = new CommandList();
			if ( !(*_ppCommandList)->Initialise( m_pDevice, D3D12_COMMAND_LIST_TYPE_DIRECT, _pDebugName ) )
				return false;

			return true;
		}

		bool DeviceD3D12::CreateDescriptorHeap( const D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap** _pDescriptorHeap, const UINT _numBuffers, const D3D12_DESCRIPTOR_HEAP_FLAGS _flags, const wchar_t* _pDebugName ) const
		{
			if ( !(*_pDescriptorHeap) )
			{
				*_pDescriptorHeap = new DescriptorHeap();
			}

			if ( !(*_pDescriptorHeap)->Initialise( m_pDevice, _type, _numBuffers, _flags, _pDebugName ) )
				return false;

			return true;
		}

		bool DeviceD3D12::CreateSwapChain( SwapChain** _ppSwapChain, GameWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName )
		{
			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_RTV, &m_pDescHeapRtv, _numBackBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName ) )
				return false;

			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_DSV, &m_pDescHeapDsv, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, _pDebugName ) )
				return false;

			*_ppSwapChain = new SwapChain();
			if ( !(*_ppSwapChain)->Initialise( m_pDevice, m_pDxgiFactory, CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_DIRECT ), _numBackBuffers, m_pDescHeapRtv, m_pDescHeapDsv, _pWindow ) )
				return false;

			return true;
		}

		ISamplerState* DeviceD3D12::CreateSamplerState( const SamplerStateFilter _eFilter, const SamplerStateWrapMode _eWrap, const SamplerStateComparisonFunction _eCompFunc ) const
		{
			SamplerStateD3D12* pSamplerState = new SamplerStateD3D12();
			if ( !pSamplerState->Initialise( m_pDevice.Get(), m_pDescHeapSampler, _eFilter, _eWrap, _eCompFunc ) )
			{
				delete pSamplerState;
				return nullptr;
			}
			return pSamplerState;
		}

		IBufferResource* DeviceD3D12::CreateTexture2D( const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName ) const
		{
			return new Texture2DResource( _pWstrFilename, true, m_pDescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName );
		}

		IBufferResource* DeviceD3D12::CreateWicTexture2D( const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName ) const
		{
			return new Texture2DResource( _pWstrFilename, false, m_pDescHeapSrvCbv, m_pDevice.Get(), _pCommandList, _pDebugName );
		}

		IBufferResource* DeviceD3D12::CreateVertexBufferResource( CommandList* _pCommandList, const UINT _sizeInBytes, const UINT _strideInBytes, const D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName ) const
		{
			return new VertexBufferResource( m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName );
		}

		IBufferResource* DeviceD3D12::CreateIndexBufferResource( CommandList* _pCommandList, const UINT _sizeInBytes, const UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName ) const
		{
			return new IndexBufferResource( m_pDevice.Get(), _pCommandList, _sizeInBytes, _strideInBytes, _flags, _pData, _pDebugName );
		}

		ConstantBufferResource* DeviceD3D12::CreateConstantBufferResource( const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName ) const
		{
			return new ConstantBufferResource( m_pDevice.Get(), m_pDescHeapSrvCbv, _params, _pDebugName );
		}

		bool DeviceD3D12::GetRootSignature( IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName )
		{
			const GpuResourceTable& resources = *m_DeviceState.Resources;
			const unsigned long     ulHash    = Hashing::SimpleHash( (const char*)resources.GetVShader()->GetShaderName(), strlen( resources.GetVShader()->GetShaderName() ) );
			if ( m_mapRootSignatures.find( ulHash ) != m_mapRootSignatures.end() )
			{
				(*_ppRootSignature) = m_mapRootSignatures[ulHash];
			}
			else
			{
				RenderMarker profile( GetImmediateContext(), "ID3D12Device::CreateRootSignature" );

				ISamplerState**     pSamplers  = nullptr;
				IGpuBufferResource**ppBuffers  = nullptr,**ppTextures = nullptr;
				const unsigned long ulSamplers = resources.GetSamplers( &pSamplers );
				const unsigned long ulTextures = resources.GetTextures( &ppTextures );
				const unsigned long ulCBuffers = resources.GetConstantBuffers( &ppBuffers );

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
				VALIDATE_D3D( D3D12SerializeVersionedRootSignature(&rootSig, &pSerializedRootSig, nullptr) );

				ID3D12RootSignature* pRootSig = nullptr;
				VALIDATE_D3D( m_pDevice->CreateRootSignature(0, pSerializedRootSig->GetBufferPointer(), pSerializedRootSig->GetBufferSize(), IID_PPV_ARGS(&pRootSig)) );
				pRootSig->SetName( _pDebugName );

				m_mapRootSignatures[ulHash] = pRootSig;
				(*_ppRootSignature)         = pRootSig;
			}

			return true;
		}

		bool DeviceD3D12::GetPipelineState( ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName )
		{
			if ( !m_DeviceState.Resources )
			{
				LogError( "No Valid Resource Table Flushed to Device State" );
				return false;
			}

			ID3D12RootSignature* pRootSignature = nullptr;
			if ( !GetRootSignature( m_DeviceState.Resources->GetVShader(), &pRootSignature ) )
			{
				LogError( "Root Signature Creation Failed." );
				return false;
			}

			m_DeviceState.RootSignatureUpdates++;
			GetImmediateContext()->SetGraphicsRootSignature( pRootSignature );

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
			GenerateInputLayout( m_DeviceState.Resources->GetVShader(), &inputLayout );

			unsigned long ulHash = 0;
			// Parameterised
			{
				LOW_LEVEL_PROFILE_MARKER( GetImmediateContext(), "GetPipelineState::Hash Ident" );

				stateDesc.InputLayout    = {&inputLayout[0], static_cast<UINT>(inputLayout.size())};
				stateDesc.pRootSignature = pRootSignature;
				stateDesc.VS             = {m_DeviceState.Resources->GetVShader()->GetBytecode(), m_DeviceState.Resources->GetVShader()->GetBytecodeSize()};
				stateDesc.PS             = {m_DeviceState.Resources->GetPShader()->GetBytecode(), m_DeviceState.Resources->GetPShader()->GetBytecodeSize()};

				ulHash += Hashing::SimpleHash( reinterpret_cast<const char*>(&inputLayout[0]), sizeof( D3D12_INPUT_ELEMENT_DESC ) * inputLayout.size() );
				ulHash += Hashing::SimpleHash( reinterpret_cast<const char*>(&stateDesc.pRootSignature), sizeof( ID3D12RootSignature ) );
				ulHash += Hashing::SimpleHash( m_DeviceState.Resources->GetVShader()->GetShaderName(), strlen( m_DeviceState.Resources->GetVShader()->GetShaderName() ) );
				ulHash += Hashing::SimpleHash( m_DeviceState.Resources->GetPShader()->GetShaderName(), strlen( m_DeviceState.Resources->GetPShader()->GetShaderName() ) );
				ulHash += Hashing::SimpleHash( reinterpret_cast<const char*>(&stateDesc.RasterizerState), sizeof( D3D12_RASTERIZER_DESC ) );
				ulHash += Hashing::SimpleHash( reinterpret_cast<const char*>(&stateDesc.BlendState), sizeof( D3D12_BLEND_DESC ) );
				ulHash += Hashing::SimpleHash( reinterpret_cast<const char*>(&stateDesc.DepthStencilState), sizeof( D3D12_DEPTH_STENCIL_DESC ) );
			}

			if ( m_mapPso.find( ulHash ) != m_mapPso.end() )
			{
				(*_ppPipelineState) = m_mapPso[ulHash];
			}
			else
			{
				LOW_LEVEL_PROFILE_MARKER( GetImmediateContext(), "ID3D12Device::CreatePipelineState" );

				ID3D12PipelineState* pPso = nullptr;
				VALIDATE_D3D( m_pDevice->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(&pPso)) );
				pPso->SetName( _pDebugName );

				m_mapPso[ulHash]    = pPso;
				(*_ppPipelineState) = pPso;
			}

			return true;
		}

		bool DeviceD3D12::CreateSamplerState( const D3D12_SAMPLER_DESC* _pSamplerDesc, const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName ) const
		{
			m_pDevice->CreateSampler( _pSamplerDesc, _cpuHandle );
			return true;
		}

		void DeviceD3D12::BeginFrame( void )
		{
			m_pImmediateContext = CommandList::Build( D3D12_COMMAND_LIST_TYPE_DIRECT, L"ImmediateContext" );
			m_pImmediateContext->Reset();

			m_DeviceState.SetDirty( kDirtyPipelineState );

			constexpr unsigned int kElements = 2000;
			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pActiveResourceHeap, kElements, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"Temp Texture Heap" ) )
			{
				assert( false && "Temp Texture Heap Creation Failure" );
			}

			if ( !CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, &m_pActiveSamplerHeap, kElements, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"Temp Sampler Heap" ) )
			{
				assert( false && "Temp Texture Heap Creation Failure" );
			}
		}

		void DeviceD3D12::EndFrame( void )
		{
			m_DeviceState.ConstantBufferUpdates = 0;
			m_DeviceState.SamplerStateUpdates   = 0;
			m_DeviceState.TextureUpdates        = 0;
			m_DeviceState.RenderTargetUpdates   = 0;
			m_DeviceState.DepthBufferUpdates    = 0;
			m_DeviceState.RootSignatureUpdates  = 0;
			m_DeviceState.PipelineStateUpdates  = 0;
			m_DeviceState.ShaderUpdates         = 0;

			m_pActiveResourceHeap->~DescriptorHeap();
			m_pActiveSamplerHeap->~DescriptorHeap();
		}

		bool DeviceD3D12::FlushState()
		{
			const CommandList*      pGfxCmdList = GetImmediateContext();
			const GpuResourceTable& resources   = *m_DeviceState.Resources;

			LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "FlushState" );

			if ( m_DeviceState.IsDirty( kDirtyPipelineState ) )
			{
				ID3D12PipelineState* pPso = nullptr;
				if ( !GetPipelineState( &pPso ) )
					return false;

				pGfxCmdList->SetPipelineState( pPso );
				m_DeviceState.PipelineStateUpdates++;
			}

			ID3D12DescriptorHeap* pHeaps[] = {m_pActiveResourceHeap->GetHeap(), m_pActiveSamplerHeap->GetHeap()};
			pGfxCmdList->SetDescriptorHeaps( pHeaps, _countof( pHeaps ) );

			const unsigned int uiResourceStartIndex = m_pActiveResourceHeap->GetFreeIndex();

			//
			// Copy CBVs
			//
			IGpuBufferResource** ppCBs       = nullptr;
			unsigned long        ulResources = resources.GetConstantBuffers( &ppCBs );
			for ( unsigned int i = 0; i < ulResources; ++i )
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource( m_pActiveResourceHeap->GetCpuStartHandle(), m_pActiveResourceHeap->GetFreeIndexAndIncrement(), m_pActiveResourceHeap->GetIncrementSize() );

				if ( !ppCBs[i] )
				{
					LogWarning( "Null Constant Buffer" );
					continue;
				}

				CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppCBs[i]->GetCpuHandle();

				UINT size1 = 1;
				UINT size2 = 1;

				LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "CBV Desc Copies" );
				m_pDevice->CopyDescriptors( 1, &hCpuNewResource, &size1, 1, &hCpuActual, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
			}

			// 
			// Copy SRVs
			//
			IGpuBufferResource** ppTextures = nullptr;
			ulResources                     = resources.GetTextures( &ppTextures );
			for ( unsigned int i = 0; i < ulResources; ++i )
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuNewResource( m_pActiveResourceHeap->GetCpuStartHandle(), m_pActiveResourceHeap->GetFreeIndexAndIncrement(), m_pActiveResourceHeap->GetIncrementSize() );

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

				CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuActual = ppTextures[i]->GetCpuHandle();

				UINT size1 = 1;
				UINT size2 = 1;

				LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "SRV Desc Copies" );
				m_pDevice->CopyDescriptors( 1, &hCpuNewResource, &size1, 1, &hCpuActual, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
			}

			//
			// Copy Sampler
			//			
			ISamplerState** pSamplers              = nullptr;
			ulResources                            = resources.GetSamplers( &pSamplers );
			const unsigned int uiSamplerStartIndex = m_pActiveSamplerHeap->GetFreeIndex();
			for ( unsigned int i = 0; i < ulResources; ++i )
			{
				CD3DX12_CPU_DESCRIPTOR_HANDLE sampDescHandle( m_pDescHeapSampler->GetCpuStartHandle() );
				sampDescHandle.Offset( static_cast<SamplerStateD3D12*>(pSamplers[i])->GetHeapIndex(), m_pDescHeapSampler->GetIncrementSize() );

				CD3DX12_CPU_DESCRIPTOR_HANDLE tempHandleLoc( m_pActiveSamplerHeap->GetCpuStartHandle(), m_pActiveSamplerHeap->GetFreeIndexAndIncrement(), m_pActiveSamplerHeap->GetIncrementSize() );

				UINT size1 = 1;
				UINT size2 = 1;

				LOW_LEVEL_PROFILE_MARKER( pGfxCmdList, "Sampler Desc Copies" );
				m_pDevice->CopyDescriptors( 1, &tempHandleLoc, &size1, 1, &sampDescHandle, &size2, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
			}

			pGfxCmdList->SetGraphicsRootDescriptorTable( 0, CD3DX12_GPU_DESCRIPTOR_HANDLE( m_pActiveResourceHeap->GetGpuStartHandle(), uiResourceStartIndex, m_pActiveResourceHeap->GetIncrementSize() ) );
			pGfxCmdList->SetGraphicsRootDescriptorTable( 1, CD3DX12_GPU_DESCRIPTOR_HANDLE( m_pActiveSamplerHeap->GetGpuStartHandle(), uiSamplerStartIndex, m_pActiveSamplerHeap->GetIncrementSize() ) );

			m_DeviceState.DirtyFlags = 0;

			return true;
		}

		bool DeviceD3D12::SetMaterial( const char* _pName )
		{
			const unsigned long ulHash = Hashing::SimpleHash( _pName, strlen( _pName ) );
			if ( m_mapGpuResourceTables.find( ulHash ) != m_mapGpuResourceTables.end() )
			{
				if ( m_DeviceState.Resources != m_mapGpuResourceTables[ulHash] )
				{
					m_DeviceState.Resources = m_mapGpuResourceTables[ulHash];

					m_DeviceState.ShaderUpdates++;
					m_DeviceState.SetDirty( kDirtyShaders | kDirtyPipelineState );
				}
				return true;
			}

			const Effect* set = ShaderCache::Instance()->GetEffect( _pName );
			if ( !set )
			{
				return false;
			}

			GpuResourceTable* pGpuResourceTable = new GpuResourceTable( set->GetVertexShader(), set->GetPixelShader() );
			m_mapGpuResourceTables[ulHash]      = pGpuResourceTable;
			m_DeviceState.Resources             = pGpuResourceTable;

			m_DeviceState.ShaderUpdates++;
			m_DeviceState.SetDirty( kDirtyShaders | kDirtyPipelineState );

			return true;
		}

		bool DeviceD3D12::SetRenderTarget( void )
		{
			m_DeviceState.RenderTargetUpdates++;
			m_DeviceState.SetDirty( kDirtyRenderTarget );
			return true;
		}

		bool DeviceD3D12::SetDepthBuffer( void )
		{
			m_DeviceState.DepthBufferUpdates++;
			m_DeviceState.SetDirty( kDirtyDepthBuffer );
			return true;
		}

		bool DeviceD3D12::SetTexture( const char* _pName, IGpuBufferResource* _pTexture )
		{
			if ( !m_DeviceState.Resources )
			{
				LogError( "SetTexture - No Valid Resource Table Flushed to Device State" );
				return false;
			}

			if ( m_DeviceState.Resources->GetTexture( _pName ) != _pTexture )
			{
				m_DeviceState.SetDirty( kDirtyTexture );
				return m_DeviceState.Resources->SetTexture( _pName, _pTexture );
			}

			return true;
		}

		bool DeviceD3D12::SetConstantBuffer( const char* _pName, IGpuBufferResource* _pCBuffer )
		{
			if ( !m_DeviceState.Resources )
			{
				LogError( "SetConstantBuffer - No Valid Resource Table Flushed to Device State" );
				return false;
			}

			if ( m_DeviceState.Resources->GetConstantBuffer( _pName ) != _pCBuffer )
			{
				m_DeviceState.SetDirty( kDirtyConstantBuffer );
				return m_DeviceState.Resources->SetConstantBuffer( _pName, _pCBuffer );
			}

			return true;
		}

		bool DeviceD3D12::SetSamplerState( const char* _pName, ISamplerState* _pSamplerState )
		{
			if ( !m_DeviceState.Resources )
			{
				LogError( "SetSamplerState - No Valid Resource Table Flushed to Device State" );
				return false;
			}

			if ( m_DeviceState.Resources->GetSampler( _pName ) != _pSamplerState )
			{
				m_DeviceState.SetDirty( kDirtySamplerState );
				return m_DeviceState.Resources->SetSamplerState( _pName, _pSamplerState );
			}

			return true;
		}

		bool Matches( const D3D12_RASTERIZER_DESC _left, const D3D12_RASTERIZER_DESC _right )
		{
			return memcmp( &_left, &_right, sizeof( D3D12_RASTERIZER_DESC ) ) == 0;
		}

		bool Matches( const D3D12_BLEND_DESC _left, const D3D12_BLEND_DESC _right )
		{
			return memcmp( &_left, &_right, sizeof( D3D12_BLEND_DESC ) ) == 0;
		}

		bool Matches( const D3D12_DEPTH_STENCIL_DESC _left, const D3D12_DEPTH_STENCIL_DESC _right )
		{
			return memcmp( &_left, &_right, sizeof( D3D12_DEPTH_STENCIL_DESC ) ) == 0;
		}

		bool DeviceD3D12::SetRasterizerState( const D3D12_RASTERIZER_DESC _desc )
		{
			if ( !Matches( m_DeviceState.RasterizerState, _desc ) )
			{
				m_DeviceState.RasterizerState = _desc;
				m_DeviceState.SetDirty( kDirtyPipelineState );
			}

			return true;
		}

		bool DeviceD3D12::SetBlendState( const D3D12_BLEND_DESC _desc )
		{
			if ( !Matches( m_DeviceState.BlendState, _desc ) )
			{
				m_DeviceState.BlendState = _desc;
				m_DeviceState.SetDirty( kDirtyPipelineState );
			}

			return true;
		}

		bool DeviceD3D12::SetDepthStencilState( const D3D12_DEPTH_STENCIL_DESC _desc )
		{
			if ( !Matches( m_DeviceState.DepthStencilState, _desc ) )
			{
				m_DeviceState.DepthStencilState = _desc;
				m_DeviceState.SetDirty( kDirtyPipelineState );
			}

			return true;
		}
	}
}
