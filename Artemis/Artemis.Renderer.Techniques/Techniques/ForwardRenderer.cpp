//#include "Defines.h"
#include "ForwardRenderer.h"

#include <assert.h>
#include <DirectXMath.h>
#include <fstream>

#include "Factories/CommandQueueFactory.h"
#include "Factories/GraphicsDeviceFactory.h"
#include "Factories/CommandListFactory.h"

#include "Interfaces/ISwapChain.h"
#include "Interfaces/ICommandList.h"
#include "Interfaces/IBufferResource.h"
#include "Interfaces/IMaterial.h"

#include "Loaders/CLParser.h"

#include <rapidxml/rapidxml_print.hpp>
#include <rapidxml/rapidxml_utils.hpp>

//#include "Loaders/AssimpLoader.h"
//#include <WICTextureLoader.h>

//#include "Window/GameWindow.h"

#include "Memory/MemoryGlobalTracking.h"

//#include "Loaders/CLParser.h"
//#include "Profiling/ProfileMarker.h"

#include "Loading/AssimpLoader.h"
#include "Scene/Camera.h"
#include "Scene/RenderEntity.h"

#include "Cache/ShaderCache.h"
#include "Constants/ConstantTable.h"

#include "Debug/ImGUIEngine.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace Artemis::Core;
using namespace Artemis::Memory;
using namespace Artemis::Utilities;

#define CONTENT_PATH std::string("Content\\")
#define SHADERS_PATH CONTENT_PATH + std::string("Shaders\\*")
#define MODELS_PATH	CONTENT_PATH + std::string("Models\\")
#define SCENES_PATH CONTENT_PATH + std::string("Scenes\\")
#define BACK_BUFFERS 2

namespace Artemis::Renderer::Techniques
{
	ForwardRenderer::ForwardRenderer( void ):
		m_bNewModelsLoaded( false ),
		m_pDevice( nullptr )
	{
		m_pSwapChain = nullptr;
	}

	ForwardRenderer::~ForwardRenderer( void )
	{
		if ( m_pSwapChain )
			delete m_pSwapChain;
		m_pSwapChain = nullptr;
	}

	bool ForwardRenderer::Initialise( GameWindow* _pWindow )
	{
		m_pDevice = Artemis::Renderer::Device::Factories::GraphicsDeviceFactory::Construct( CLParser::Instance()->HasArgument( "d3ddebug" ) );

		m_pGfxCmdQueue = Artemis::Renderer::Device::Factories::CommandQueueFactory::Construct();
		m_pDevice->CreateCommandQueue( Interfaces::CommandListType_Direct, &m_pGfxCmdQueue, L"Gfx" );

		m_pCpyCmdQueue = Artemis::Renderer::Device::Factories::CommandQueueFactory::Construct();
		m_pDevice->CreateCommandQueue( Interfaces::CommandListType_Copy, &m_pCpyCmdQueue, L"Cpy" );

		if ( !m_pDevice->CreateSwapChain( &m_pSwapChain, m_pGfxCmdQueue, _pWindow, BACK_BUFFERS, L"Swap Chain" ) )
			return false;

		Artemis::Renderer::Shaders::ShaderCache::Instance()->Load( SHADERS_PATH );

		if ( !m_pDevice->CreateDescriptorHeap( Interfaces::DescriptorHeapType_CbvSrvUav, &m_pImGuiSrvHeap, Interfaces::DescriptorHeapFlags_ShaderVisible, 1, L"ImGUI SRV" ) )
			return false;

		ImGUIEngine::Initialise( _pWindow, m_pDevice, m_pImGuiSrvHeap );

		if ( !LoadContent() )
			return false;

		return true;
	}

	struct ModelDefinition
	{
		const char* MeshFilename = nullptr;
		const char* ObjectName   = nullptr;
		const char* MaterialName = nullptr;
		float       Position[3]  = {0};
		float       Scale        = 1.0f;
	};

	bool ForwardRenderer::LoadScene( const std::string& _sceneFile )
	{
		const std::string        pScenePath = std::string( "Content\\Scenes\\" ) + _sceneFile;
		rapidxml::file<>         xmlFile( pScenePath.c_str() );
		rapidxml::xml_document<> doc;
		doc.parse<0>( xmlFile.data() );

		const rapidxml::xml_node<>* root    = doc.first_node( "Root" );
		const rapidxml::xml_node<>* cameras = root->first_node( "Cameras" );
		const rapidxml::xml_node<>* camera  = cameras->first_node( "Camera" );
		while ( camera != nullptr )
		{
			Artemis::Renderer::Assets::Camera* pCamera = new Artemis::Renderer::Assets::Camera();
			const rapidxml::xml_node<>*        posNode = camera->first_node( "Position" );
			const rapidxml::xml_node<>*        tarNode = camera->first_node( "Target" );
			const rapidxml::xml_node<>*        upNode  = camera->first_node( "Up" );
			const rapidxml::xml_node<>*        fovNode = camera->first_node( "FOV" );

			float x = static_cast<float>(atof( posNode->first_attribute( "X" )->value() ));
			float y = static_cast<float>(atof( posNode->first_attribute( "Y" )->value() ));
			float z = static_cast<float>(atof( posNode->first_attribute( "Z" )->value() ));
			pCamera->SetPosition( x, y, z );

			x = static_cast<float>(atof( tarNode->first_attribute( "X" )->value() ));
			y = static_cast<float>(atof( tarNode->first_attribute( "Y" )->value() ));
			z = static_cast<float>(atof( tarNode->first_attribute( "Z" )->value() ));
			pCamera->SetTarget( x, y, z );

			x = static_cast<float>(atof( upNode->first_attribute( "X" )->value() ));
			y = static_cast<float>(atof( upNode->first_attribute( "Y" )->value() ));
			z = static_cast<float>(atof( upNode->first_attribute( "Z" )->value() ));
			pCamera->SetUp( x, y, z );

			x = static_cast<float>(atof( fovNode->first_attribute( "Value" )->value() ));
			y = static_cast<float>(atof( CLParser::Instance()->GetArgument( "Width" ) ));
			z = static_cast<float>(atof( CLParser::Instance()->GetArgument( "Height" ) ));
			pCamera->SetFieldOfView( x );
			pCamera->SetAspectRatio( y / z );

			m_vpCameras.push_back( pCamera );

			camera = camera->next_sibling( "Camera" );
		}

		const rapidxml::xml_node<>* lights = root->first_node( "Lights" );
		const rapidxml::xml_node<>* light  = lights->first_node( "Light" );
		while ( light != nullptr )
		{
			Light*                      pLight   = new Light();
			const rapidxml::xml_node<>* posNode  = light->first_node( "Position" );
			const rapidxml::xml_node<>* diffuse  = light->first_node( "Diffuse" );
			const rapidxml::xml_node<>* ambient  = light->first_node( "Ambient" );
			const rapidxml::xml_node<>* specular = light->first_node( "Specular" );

			const float x    = static_cast<float>(atof( posNode->first_attribute( "X" )->value() ));
			const float y    = static_cast<float>(atof( posNode->first_attribute( "Y" )->value() ));
			const float z    = static_cast<float>(atof( posNode->first_attribute( "Z" )->value() ));
			pLight->Position = XMFLOAT3( x, y, z );

			float r         = static_cast<float>(atof( diffuse->first_attribute( "R" )->value() ));
			float g         = static_cast<float>(atof( diffuse->first_attribute( "G" )->value() ));
			float b         = static_cast<float>(atof( diffuse->first_attribute( "B" )->value() ));
			float a         = static_cast<float>(atof( diffuse->first_attribute( "A" )->value() ));
			pLight->Diffuse = XMFLOAT4( r, g, b, a );

			r               = static_cast<float>(atof( ambient->first_attribute( "R" )->value() ));
			g               = static_cast<float>(atof( ambient->first_attribute( "G" )->value() ));
			b               = static_cast<float>(atof( ambient->first_attribute( "B" )->value() ));
			a               = static_cast<float>(atof( ambient->first_attribute( "A" )->value() ));
			pLight->Ambient = XMFLOAT4( r, g, b, a );

			r                = static_cast<float>(atof( specular->first_attribute( "R" )->value() ));
			g                = static_cast<float>(atof( specular->first_attribute( "G" )->value() ));
			b                = static_cast<float>(atof( specular->first_attribute( "B" )->value() ));
			a                = static_cast<float>(atof( specular->first_attribute( "A" )->value() ));
			pLight->Specular = XMFLOAT4( r, g, b, a );

			const float power     = static_cast<float>(atof( specular->first_attribute( "Power" )->value() ));
			pLight->SpecularPower = power;

			m_vpLights.push_back( pLight );

			light = light->next_sibling( "Light" );
		}

		Interfaces::ICommandList* pList = Artemis::Renderer::Device::Factories::CommandListFactory::Construct();
		m_pDevice->CreateCommandList( Interfaces::ECommandListType::CommandListType_Copy, &pList, L"CpyList" );
		pList->Reset();

		const rapidxml::xml_node<>* instances = root->first_node( "ModelInstances" );
		const rapidxml::xml_node<>* instance  = instances->first_node( "Instance" );
		while ( instance != nullptr )
		{
			Artemis::Renderer::Assets::RenderEntity* pInstance = new Artemis::Renderer::Assets::RenderEntity();

			const std::string pModelPath = std::string( "Content\\Models\\" ) + std::string( instance->first_attribute( "ModelPath" )->value() );
			pInstance->SetModelName( instance->first_attribute( "ModelName" )->value() );
			pInstance->LoadModelFromFile( pModelPath.c_str(), m_pDevice, pList );
			pInstance->SetMaterial( instance->first_attribute( "Material" )->value() );
			pInstance->SetConstantBuffer( Artemis::Renderer::Shaders::ConstantTable::Instance()->CreateConstantBuffer( "ObjectCB", m_pDevice ) );
			pInstance->SetRotation( 0.0f, 0.0f, 0.0f );

			const rapidxml::xml_node<>* transform = instance->first_node( "Transform" );
			const float                 x         = static_cast<float>(atof( transform->first_attribute( "PosX" )->value() ));
			const float                 y         = static_cast<float>(atof( transform->first_attribute( "PosY" )->value() ));
			const float                 z         = static_cast<float>(atof( transform->first_attribute( "PosZ" )->value() ));
			const float                 scale     = static_cast<float>(atof( transform->first_attribute( "Scale" )->value() ));
			pInstance->SetScale( scale );
			pInstance->SetPosition( x, y, z );

			m_vpRenderEntities.push_back( pInstance );

			instance = instance->next_sibling( "Instance" );
		}

		m_pCpyCmdQueue->SubmitToQueue( pList );

		return true;
	}

	bool ForwardRenderer::LoadContent( void )
	{
		m_bNewModelsLoaded = true;

		m_pMainPassCb  = Artemis::Renderer::Shaders::ConstantTable::Instance()->CreateConstantBuffer( "PassCB", m_pDevice );
		m_pLightsCb    = Artemis::Renderer::Shaders::ConstantTable::Instance()->CreateConstantBuffer( "LightCB", m_pDevice );
		m_pSpotlightCb = Artemis::Renderer::Shaders::ConstantTable::Instance()->CreateConstantBuffer( "SpotlightCB", m_pDevice );

		//LogInfo( "Loading Models:" );

		if ( !LoadScene( "SponzaScene.xml" ) )
		{
			//LogError( "Scene Loading Failed" );
			return false;
		}

		m_pCpyCmdQueue->ExecuteCommandLists();
		m_pCpyCmdQueue->Flush();

		return true;
	}

	void ForwardRenderer::Update( double _deltaTime )
	{
		UpdatePassConstants();

		for ( UINT i = 0; i < m_vpRenderEntities.size(); ++i )
		{
			m_vpRenderEntities[i]->Update();

			Object obj;
			obj.World    = m_vpRenderEntities[i]->GetWorld();
			obj.Material = m_vpRenderEntities[i]->GetMaterialData();

			if ( m_vpRenderEntities[i]->GetConstantBuffer() )
				bool bRet = m_vpRenderEntities[i]->GetConstantBuffer()->UpdateValue( "World", &obj, sizeof( Object ) );
		}
	}

	void ForwardRenderer::UpdatePassConstants() const
	{
		m_vpCameras[0]->Update();

		Pass cbPass           = Pass();
		cbPass.EyePosition    = m_vpCameras[0]->GetPosition();
		cbPass.ViewProjection = m_vpCameras[0]->GetView() * m_vpCameras[0]->GetProjection();

		bool bRet = false;
		if ( m_pMainPassCb )
			bRet = m_pMainPassCb->UpdateValue( nullptr, &cbPass, sizeof( Pass ) );

		if ( m_pLightsCb )
			bRet = m_pLightsCb->UpdateValue( nullptr, m_vpLights[0], sizeof( Light ) );

		if ( m_pSpotlightCb )
			bRet = m_pSpotlightCb->UpdateValue( nullptr, nullptr, sizeof( Spotlight ) );
	}

	bool ForwardRenderer::Render( void )
	{
		m_pDevice->BeginFrame();

		Interfaces::ICommandList* pGfxCmdList = m_pDevice->GetImmediateContext();

		// Rendering
		{
			//RenderMarker profile( pGfxCmdList, "Render" );

			m_pSwapChain->PrepareForRendering( pGfxCmdList );
			m_pSwapChain->SetOmRenderTargets( pGfxCmdList );
			pGfxCmdList->SetIaPrimitiveTopology( Interfaces::PrimitiveTopology_TriangleList );

			MainRenderPass( pGfxCmdList );
			ImGuiPass( pGfxCmdList );
		}

		// Presentation
		{
			m_pSwapChain->PrepareForPresentation( pGfxCmdList );
			m_pGfxCmdQueue->SubmitToQueue( pGfxCmdList );
			m_pGfxCmdQueue->ExecuteCommandLists();
			bool bRet = m_pSwapChain->Present();
			m_pGfxCmdQueue->Signal();
			m_pSwapChain->Swap();
			m_pGfxCmdQueue->Wait();
		}

		m_pDevice->EndFrame();

		return true;
	}

	void ForwardRenderer::ImGuiPass( Interfaces::ICommandList* _pGfxCmdList ) const
	{
#if defined(_DEBUG)
		//RenderMarker profile( _pGfxCmdList, "%s", "ImGUI" );

		//ID3D12DescriptorHeap* pHeaps[] = {m_pImGuiSrvHeap->GetHeap()};
		//_pGfxCmdList->SetDescriptorHeaps( pHeaps, _countof( pHeaps ) );

		Artemis::Renderer::Interfaces::IDescriptorHeap* pHeaps[] = {m_pImGuiSrvHeap};
		_pGfxCmdList->SetDescriptorHeaps( pHeaps, _countof( pHeaps ) );

		ImGUIEngine::Begin();

		if ( ImGui::Begin( "Main Camera" ) )
		{
			for ( unsigned int i = 0; i < m_vpCameras.size(); ++i )
			{
				float v[3];
				v[0] = m_vpCameras[i]->GetPosition().x;
				v[1] = m_vpCameras[i]->GetPosition().y;
				v[2] = m_vpCameras[i]->GetPosition().z;

				if ( ImGui::SliderFloat3( "Position:", v, -1000.0f, 1000.0f ) )
				{
					m_vpCameras[i]->SetPosition( v[0], v[1], v[2] );
				}

				v[0] = m_vpCameras[i]->GetTarget().x;
				v[1] = m_vpCameras[i]->GetTarget().y;
				v[2] = m_vpCameras[i]->GetTarget().z;
				if ( ImGui::SliderFloat3( "Target:", v, -1000.0f, 1000.0f ) )
				{
					m_vpCameras[i]->SetTarget( v[0], v[1], v[2] );
				}

				v[0] = m_vpCameras[i]->GetUp().x;
				v[1] = m_vpCameras[i]->GetUp().y;
				v[2] = m_vpCameras[i]->GetUp().z;
				if ( ImGui::SliderFloat3( "Up:", v, -1000.0f, 1000.0f ) )
				{
					m_vpCameras[i]->SetUp( v[0], v[1], v[2] );
				}
			}
			ImGui::End();
		}

		if ( ImGui::Begin( "Memory" ) )
		{
			static constexpr float kMb               = 1024 * 1024;
			float                  fTotalMemUsage    = 0.0f;
			int                    iTotalAllocations = 0;
			for ( unsigned int i = 0; i < static_cast<unsigned int>(MemoryContextCategory::ECategories); ++i )
			{
				const char*              pCatName = MemoryGlobalTracking::GetContextName( static_cast<MemoryContextCategory>(i) );
				const MemoryContextData& data     = MemoryGlobalTracking::GetContextStats( static_cast<MemoryContextCategory>(i) );

				const int iNetAllocations = data.Allocations - data.Deallocations;
				ImGui::CollapsingHeader( pCatName, ImGuiTreeNodeFlags_Bullet );
				ImGui::Text( "Net Allocations: %i", iNetAllocations );
				ImGui::Text( "Allocated Size: %0.3f MB", data.TotalAllocationSize / kMb );

				fTotalMemUsage += data.TotalAllocationSize;
				iTotalAllocations += iNetAllocations;
			}
			ImGui::CollapsingHeader( "Total Memory Usage:", ImGuiTreeNodeFlags_Bullet );
			ImGui::Text( "Size Allocation: %0.3f MB", fTotalMemUsage / kMb );
			ImGui::Text( "Net Allocations: %i", iTotalAllocations );
			ImGui::End();
		}

		if ( ImGui::Begin( "Lights" ) )
		{
			for ( unsigned int i = 0; i < m_vpLights.size(); ++i )
			{
				float v[3];
				v[0] = m_vpLights[i]->Position.x;
				v[1] = m_vpLights[i]->Position.y;
				v[2] = m_vpLights[i]->Position.z;
				if ( ImGui::SliderFloat3( "Position:", v, -180.0f, 180.0f ) )
				{
					m_vpLights[i]->Position.x = v[0];
					m_vpLights[i]->Position.y = v[1];
					m_vpLights[i]->Position.z = v[2];
				}

				v[0] = m_vpLights[i]->Diffuse.x;
				v[1] = m_vpLights[i]->Diffuse.y;
				v[2] = m_vpLights[i]->Diffuse.z;
				if ( ImGui::SliderFloat3( "Diffuse:", v, 0.0f, 1.0f ) )
				{
					m_vpLights[i]->Diffuse.x = v[0];
					m_vpLights[i]->Diffuse.y = v[1];
					m_vpLights[i]->Diffuse.z = v[2];
				}

				v[0] = m_vpLights[i]->Ambient.x;
				v[1] = m_vpLights[i]->Ambient.y;
				v[2] = m_vpLights[i]->Ambient.z;
				if ( ImGui::SliderFloat3( "Ambient:", v, 0.0f, 1.0f ) )
				{
					m_vpLights[i]->Ambient.x = v[0];
					m_vpLights[i]->Ambient.y = v[1];
					m_vpLights[i]->Ambient.z = v[2];
				}

				v[0] = m_vpLights[i]->Specular.x;
				v[1] = m_vpLights[i]->Specular.y;
				v[2] = m_vpLights[i]->Specular.z;
				if ( ImGui::SliderFloat3( "Specular:", v, 0.0f, 1.0f ) )
				{
					m_vpLights[i]->Specular.x = v[0];
					m_vpLights[i]->Specular.y = v[1];
					m_vpLights[i]->Specular.z = v[2];
				}

				float nS = m_vpLights[i]->SpecularPower;
				if ( ImGui::SliderFloat( "Specular Power:", &nS, 0.0f, 10.0f ) )
				{
					m_vpLights[i]->SpecularPower = nS;
				}
			}

			ImGui::End();
		}

		if ( ImGui::Begin( "Device Flush State Stats:" ) )
		{
			const Artemis::Renderer::Interfaces::DeviceStateStats stats = m_pDevice->GetDeviceStats();
			ImGui::Text( "Texture Updates: %lu", stats.TextureUpdates );
			ImGui::Text( "Shader Updates: %lu", stats.ShaderUpdates );
			ImGui::Text( "Render Target Updates: %lu", stats.RenderTargetUpdates );
			ImGui::Text( "Depth Buffer Updates: %lu", stats.DepthBufferUpdates );
			ImGui::Text( "Constant Buffer Updates: %lu", stats.ConstantBufferUpdates );
			ImGui::Text( "Pipeline State Updates: %lu", stats.PipelineStateUpdates );
			ImGui::Text( "Root Signature Updates: %lu", stats.RootSignatureUpdates );
			ImGui::End();
		}

		if ( ImGui::Begin( "Objects:" ) )
		{
			float v[3];
			for ( unsigned int i = 0; i < m_vpRenderEntities.size(); ++i )
			{
				if ( ImGui::CollapsingHeader( m_vpRenderEntities[i]->GetModelName() ) )
				{
					Material material = m_vpRenderEntities[i]->GetMaterialData();

					v[0] = material.Diffuse.x;
					v[1] = material.Diffuse.y;
					v[2] = material.Diffuse.z;
					if ( ImGui::SliderFloat3( "Diffuse:", v, 0.0f, 1.0f ) )
					{
						material.Diffuse.x = v[0];
						material.Diffuse.y = v[1];
						material.Diffuse.z = v[2];
					}

					v[0] = material.Ambient.x;
					v[1] = material.Ambient.y;
					v[2] = material.Ambient.z;
					if ( ImGui::SliderFloat3( "Ambient:", v, 0.0f, 1.0f ) )
					{
						material.Ambient.x = v[0];
						material.Ambient.y = v[1];
						material.Ambient.z = v[2];
					}

					v[0] = material.Specular.x;
					v[1] = material.Specular.y;
					v[2] = material.Specular.z;
					if ( ImGui::SliderFloat3( "Specular:", v, 0.0f, 1.0f ) )
					{
						material.Specular.x = v[0];
						material.Specular.y = v[1];
						material.Specular.z = v[2];
					}

					m_vpRenderEntities[i]->SetMaterialData( material );
				}
			}
			ImGui::End();
		}

		ImGUIEngine::End();
		ImGUIEngine::Draw( _pGfxCmdList );
#endif
	}

	Interfaces::BlendDesc DefaultBlendDesc()
	{
		Interfaces::BlendDesc desc;
		desc.AlphaToCoverageEnable  = false;
		desc.IndependentBlendEnable = false;

		const Interfaces::RenderTargetBlendDesc rtbDesc = {false, false, Interfaces::Blend_One, Interfaces::Blend_Zero, Interfaces::BlendOp_Add, Interfaces::Blend_One, Interfaces::Blend_Zero, Interfaces::BlendOp_Add, Interfaces::LogicOp_NoOp, Interfaces::ColourWriteEnable_All};
		for ( unsigned int i = 0; i < ARRAYSIZE( desc.RenderTarget ); ++i )
		{
			desc.RenderTarget[i] = rtbDesc;
		}

		return desc;
	}

	Interfaces::RasteriserStateDesc DefaultRasteriserStateDesc()
	{
		Interfaces::RasteriserStateDesc desc;
		desc.FillMode              = Interfaces::FillMode_Solid;
		desc.CullMode              = Interfaces::CullMode_Back;
		desc.FrontCounterClockwise = false;
		desc.DepthBias             = 0;
		desc.DepthBiasClamp        = 0.0f;
		desc.SlopeScaledDepthBias  = 0.0f;
		desc.DepthClipEnable       = true;
		desc.MultisampleEnable     = false;
		desc.AntialiasedLineEnable = false;
		desc.ForcedSampleCount     = 0;
		desc.ConservativeRaster    = Interfaces::ConservativeRasterisationMode_On;
		return desc;
	}

	Interfaces::DepthStencilDesc DefaultDepthStencilDesc()
	{
		Interfaces::DepthStencilDesc desc;
		desc.DepthEnable                                          = true;
		desc.DepthWriteMask                                       = Interfaces::DepthWriteMask_All;
		desc.DepthFunc                                            = Interfaces::ComparisonFunc_Less;
		desc.StencilEnable                                        = false;
		desc.StencilReadMask                                      = 0xFF;
		desc.StencilWriteMask                                     = 0xFF;
		constexpr Interfaces::DepthStencilOpDesc defaultStencilOp = {Interfaces::StencilOp_Keep, Interfaces::StencilOp_Keep, Interfaces::StencilOp_Keep, Interfaces::ComparisonFunc_Always};
		desc.FrontFace                                            = defaultStencilOp;
		desc.BackFace                                             = defaultStencilOp;
		return desc;
	}

	void ForwardRenderer::MainRenderPass( const Interfaces::ICommandList* _pGfxCmdList ) const
	{
		//RenderMarker profile( _pGfxCmdList, "MainRenderPass" );

		m_pDevice->SetBlendState( DefaultBlendDesc() );
		m_pDevice->SetRasterizerState( DefaultRasteriserStateDesc() );
		m_pDevice->SetDepthStencilState( DefaultDepthStencilDesc() );

		// Per Object Draws
		for ( UINT i = 0; i < m_vpRenderEntities.size(); ++i )
		{
			const Artemis::Renderer::Assets::RenderEntity* pModel = m_vpRenderEntities[i];
			if ( !pModel->GetModel() )
			{
				continue;
			}

			Interfaces::IGpuResource* pModelCb = pModel->GetConstantBuffer();

			Artemis::Renderer::Shaders::Effect* effect = Artemis::Renderer::Shaders::ShaderCache::Instance()->GetEffect( pModel->GetMaterialName() );

			Artemis::Renderer::Interfaces::IMaterial* mat = new Artemis::Renderer::Interfaces::IMaterial();
			mat->m_pVertexShader                          = effect->GetVertexShader();
			mat->m_pPixelShader                           = effect->GetPixelShader();

			m_pDevice->SetMaterial( mat );
			m_pDevice->SetSamplerState( "Albedo", m_pDevice->GetDefaultSamplerState() );
			m_pDevice->SetSamplerState( "Normal", m_pDevice->GetDefaultSamplerState() );

			m_pDevice->SetConstantBuffer( "ObjectCB", pModelCb );
			m_pDevice->SetConstantBuffer( "PassCB", m_pMainPassCb );
			m_pDevice->SetConstantBuffer( "LightCB", m_pLightsCb );
			//m_pDevice->SetConstantBuffer( "SpotlightCB", m_pSpotlightCb );s

			for ( UINT j = 0; j < pModel->GetModel()->MeshCount; ++j )
			{
				const Artemis::Renderer::Assets::Mesh& rMesh = pModel->GetModel()->pMeshList[j];

				m_pDevice->SetTexture( "Albedo", rMesh.pTexture[ALBEDO] );
				m_pDevice->SetTexture( "Normal", rMesh.pTexture[NORMAL] );

				if ( m_pDevice->FlushState() )
				{
					_pGfxCmdList->DrawIndexedInstanced( rMesh.pVertexBuffer, rMesh.pIndexBuffer, rMesh.Indices );
				}
			}

			delete mat;
		}
	}
}
