#include "Defines.h"
#include "RendererD3D12.h"

#include <assert.h>
#include <DirectXMath.h>
#include <fstream>

#include "d3dx12.h"

#include <rapidxml/rapidxml_print.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include "AssimpLoader.h"
#include <WICTextureLoader.h>

#include "SysCore\_Window\GameWindow.h"

#include "SysMemory\include\MemoryGlobalTracking.h"

#include "SysUtilities\_Loaders\CLParser.h"
#include "SysUtilities\_Profiling\ProfileMarker.h"

#include "Camera.h"
#include "RenderEntity.h"

#include "_ImGUI\ImGUIEngine.h"

#include "D3D12\Device\SwapChain.h"
#include "D3D12\Device\CommandQueue.h"
#include "D3D12\Shaders\ShaderCache.h"

#include "D3D12\Resources\ConstantTable.h"
#include "D3D12\Resources\DescriptorHeap.h"
#include "D3D12\Resources\Texture2DResource.h"
#include "D3D12\Resources\ConstantBufferResource.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTK12.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

using namespace SysCore;
using namespace SysMemory;
using namespace SysUtilities;

#define CONTENT_PATH std::string("Content\\")
#define SHADERS_PATH CONTENT_PATH + std::string("Shaders\\*")
#define MODELS_PATH	CONTENT_PATH + std::string("Models\\")
#define SCENES_PATH CONTENT_PATH + std::string("Scenes\\")

namespace SysRenderer
{
	using namespace D3D12;
	using namespace ImGuiUtils;
	using namespace Scene;
	using namespace Loading;

	Renderer::Renderer( void ):
		m_bNewModelsLoaded( false ),
		//m_spotlight( nullptr ),
		m_pImGuiSrvHeap( nullptr ),
		m_pLightsCb( nullptr ),
		m_pMainPassCb( nullptr )
	//m_pSpotlightCb( nullptr )
	{
		m_pSwapChain = nullptr;
	}

	Renderer::~Renderer( void )
	{
		if ( m_pSwapChain )
			delete m_pSwapChain;
		m_pSwapChain = nullptr;
	}

	bool Renderer::Initialise( GameWindow* _pWindow )
	{
		if ( !DeviceD3D12::Instance()->Initialise( CLParser::Instance()->HasArgument( "d3ddebug" ) ) )
			return false;

		ShaderCache::Instance()->Load( SHADERS_PATH );

		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_DIRECT );
		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY );

		if ( !DeviceD3D12::Instance()->CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &m_pImGuiSrvHeap, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"ImGUI SRV" ) )
			return false;

		ImGUIEngine::Initialise( _pWindow->GetWindowHandle(), m_pImGuiSrvHeap );

		if ( !DeviceD3D12::Instance()->CreateSwapChain( &m_pSwapChain, _pWindow, BACK_BUFFERS, L"Swap Chain" ) )
			return false;

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

	bool Renderer::LoadScene( const std::string& _sceneFile )
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
			Scene::Camera*              pCamera = new Scene::Camera();
			const rapidxml::xml_node<>* posNode = camera->first_node( "Position" );
			const rapidxml::xml_node<>* tarNode = camera->first_node( "Target" );
			const rapidxml::xml_node<>* upNode  = camera->first_node( "Up" );
			const rapidxml::xml_node<>* fovNode = camera->first_node( "FOV" );

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

		const rapidxml::xml_node<>* instances = root->first_node( "ModelInstances" );
		const rapidxml::xml_node<>* instance  = instances->first_node( "Instance" );
		while ( instance != nullptr )
		{
			RenderEntity* pInstance = new RenderEntity();

			const std::string pModelPath = std::string( "Content\\Models\\" ) + std::string( instance->first_attribute( "ModelPath" )->value() );
			pInstance->SetModelName( instance->first_attribute( "ModelName" )->value() );
			pInstance->LoadModelFromFile( pModelPath.c_str() );
			pInstance->SetMaterial( instance->first_attribute( "Material" )->value() );
			pInstance->SetConstantBuffer( ConstantTable::Instance()->CreateConstantBuffer( "ObjectCB" ) );
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

		return true;
	}

	bool Renderer::LoadContent( void )
	{
		m_bNewModelsLoaded = true;

		m_pMainPassCb = ConstantTable::Instance()->CreateConstantBuffer( "PassCB" );
		m_pLightsCb   = ConstantTable::Instance()->CreateConstantBuffer( "LightCB" );
		//m_pSpotlightCb = ConstantTable::Instance()->CreateConstantBuffer( "SpotlightCB" );

		LogInfo( "Loading Models:" );

		if ( !LoadScene( "SponzaScene.xml" ) )
		{
			LogError( "Scene Loading Failed" );
			return false;
		}

		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY )->ExecuteCommandLists();
		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY )->Flush();

		return true;
	}

	void Renderer::Update( double _deltaTime )
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

	void Renderer::UpdatePassConstants() const
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

		//if ( m_pSpotlightCb )
		//	m_pSpotlightCb->UpdateValue( nullptr, m_spotlight, sizeof( Spotlight ) );
	}

	bool Renderer::Render( void )
	{
		DeviceD3D12::Instance()->BeginFrame();

		CommandList* pGfxCmdList = DeviceD3D12::Instance()->GetImmediateContext();

		// Rendering
		{
			RenderMarker profile( pGfxCmdList, "Render" );

			m_pSwapChain->PrepareForRendering( pGfxCmdList );
			m_pSwapChain->SetOmRenderTargets( pGfxCmdList );
			pGfxCmdList->SetIaPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

			MainRenderPass( pGfxCmdList );
			ImGuiPass( pGfxCmdList );
		}

		// Presentation
		{
			CommandQueue* pGfxCmdQueue = CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_DIRECT );

			m_pSwapChain->PrepareForPresentation( pGfxCmdList );
			pGfxCmdQueue->SubmitToQueue( pGfxCmdList );
			pGfxCmdQueue->ExecuteCommandLists();
			bool bRet = m_pSwapChain->Present();
			pGfxCmdQueue->Signal();
			m_pSwapChain->Swap();
			pGfxCmdQueue->Wait();
		}

		DeviceD3D12::Instance()->EndFrame();

		return true;
	}

	void Renderer::ImGuiPass( CommandList* _pGfxCmdList ) const
	{
#if defined(_DEBUG)
		RenderMarker profile( _pGfxCmdList, "%s", "ImGUI" );

		ID3D12DescriptorHeap* pHeaps[] = {m_pImGuiSrvHeap->GetHeap()};
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
			const DeviceD3D12::DeviceState state = DeviceD3D12::Instance()->GetDeviceState();
			ImGui::Text( "Texture Updates: %lu", state.TextureUpdates );
			ImGui::Text( "Shader Updates: %lu", state.ShaderUpdates );
			ImGui::Text( "Render Target Updates: %lu", state.RenderTargetUpdates );
			ImGui::Text( "Depth Buffer Updates: %lu", state.DepthBufferUpdates );
			ImGui::Text( "Constant Buffer Updates: %lu", state.ConstantBufferUpdates );
			ImGui::Text( "Pipeline State Updates: %lu", state.PipelineStateUpdates );
			ImGui::Text( "Root Signature Updates: %lu", state.RootSignatureUpdates );
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

	void Renderer::MainRenderPass( CommandList* _pGfxCmdList ) const
	{
		RenderMarker profile( _pGfxCmdList, "MainRenderPass" );

		DeviceD3D12* pDevice = DeviceD3D12::Instance();

		pDevice->SetBlendState( CD3DX12_BLEND_DESC( D3D12_DEFAULT ) );
		pDevice->SetRasterizerState( CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT ) );
		pDevice->SetDepthStencilState( CD3DX12_DEPTH_STENCIL_DESC( D3D12_DEFAULT ) );

		// Per Object Draws
		for ( UINT i = 0; i < m_vpRenderEntities.size(); ++i )
		{
			const RenderEntity* pModel = m_vpRenderEntities[i];
			if ( !pModel->GetModel() )
			{
				continue;
			}

			ConstantBufferResource* pModelCb = pModel->GetConstantBuffer();

			pDevice->SetMaterial( pModel->GetMaterialName() );
			pDevice->SetSamplerState( "Albedo", pDevice->GetDefaultSamplerState() );
			pDevice->SetSamplerState( "Normal", pDevice->GetDefaultSamplerState() );

			pDevice->SetConstantBuffer( "ObjectCB", pModelCb );
			pDevice->SetConstantBuffer( "PassCB", m_pMainPassCb );
			pDevice->SetConstantBuffer( "LightCB", m_pLightsCb );
			//pDevice->SetConstantBuffer( "SpotlightCB", m_pSpotlightCb );

			for ( UINT j = 0; j < pModel->GetModel()->MeshCount; ++j )
			{
				const Mesh& rMesh = pModel->GetModel()->pMeshList[j];

				pDevice->SetTexture( "Albedo", static_cast<Texture2DResource*>(rMesh.pTexture[ALBEDO]) );
				pDevice->SetTexture( "Normal", static_cast<Texture2DResource*>(rMesh.pTexture[NORMAL]) );

				if ( pDevice->FlushState() )
				{
					auto vbView = static_cast<VertexBufferResource*>(rMesh.pVertexBuffer)->GetView();
					auto ibView = static_cast<IndexBufferResource*>(rMesh.pIndexBuffer)->GetView();
					_pGfxCmdList->SetIaVertexBuffers( 0, 1, &vbView );
					_pGfxCmdList->SetIaIndexBuffer( &ibView );
					_pGfxCmdList->DrawIndexedInstanced( rMesh.Indices, 1, 0, 0, 0 );
				}
			}
		}
	}
}
