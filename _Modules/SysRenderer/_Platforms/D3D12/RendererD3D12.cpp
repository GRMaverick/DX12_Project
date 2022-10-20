#include "Defines.h"
#include "RendererD3D12.h"

#include <assert.h>
#include <DirectXMath.h>

#include "d3dx12.h"

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

#if defined(_DEBUG)
#	define SHADER_CACHE_LOCATION "Shaders\\*"
#	define CONTENT_LOCATION "Content\\"
#else
#	define SHADER_CACHE_LOCATION "Shaders\\*"
#	define CONTENT_LOCATION "Content\\"
#endif

namespace SysRenderer
{
	using namespace D3D12;
	using namespace ImGuiUtils;
	using namespace Scene;
	using namespace Loading;

	Renderer::Renderer( void ):
		m_uiModelCount( 0 ),
		m_bNewModelsLoaded( false ),
		m_light( nullptr ),
		m_spotlight( nullptr ),
		m_pCamera( nullptr ),
		m_pRenderEntity( nullptr ),
		m_pImGuiSrvHeap( nullptr ),
		m_pLightsCb( nullptr ),
		m_pMainPassCb( nullptr ),
		m_pSpotlightCb( nullptr )
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

		ShaderCache::Instance()->Load( SHADER_CACHE_LOCATION );

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

#define SPONZA
	//#define CUBES

	ModelDefinition g_ModelList[] = {
#if defined(SPONZA)
		{"Sponza\\Sponza.fbx", "Scene", "AlbedoPhongNormal", {0.0f, 0.0f, 0.0f}, 1.0f}
#endif
#if defined(CUBES)
		{ "Cube\\Cube.obj", "Cube_Albedo", "Albedo", {0.0f, 0.0f, -1.25f}, 1.0f },
		{ "StorageContainer\\Cube.obj", "Storage_Phong", "AlbedoPhong", {1.25f, 0.0f, 0.0f}, 1.0f },
		{ "StorageContainer\\Cube.obj", "Storage_Normal", "AlbedoPhongNormal", {-1.25f, 0.0f, 0.0f}, 1.0f },
#endif
	};

	bool Renderer::LoadContent( void )
	{
		m_bNewModelsLoaded = true;

		m_pMainPassCb  = ConstantTable::Instance()->CreateConstantBuffer( "PassCB" );
		m_pLightsCb    = ConstantTable::Instance()->CreateConstantBuffer( "LightCB" );
		m_pSpotlightCb = ConstantTable::Instance()->CreateConstantBuffer( "SpotlightCB" );

		Material material;
		material.Diffuse  = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		material.Ambient  = XMFLOAT4( 0.25f, 0.25f, 0.25f, 1.0f );
		material.Specular = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );

		LogInfo( "Loading Models:" );
		m_pRenderEntity = new RenderEntity*[_countof( g_ModelList )];
		for ( UINT i = 0; i < _countof( g_ModelList ); ++i )
		{
			LogInfo( "\t%s", g_ModelList[i].MeshFilename );

			char pModelPath[128];
			snprintf( pModelPath, ARRAYSIZE( pModelPath ), "%s\\%s", CONTENT_LOCATION, g_ModelList[i].MeshFilename );

			m_pRenderEntity[i] = new RenderEntity();
			m_pRenderEntity[i]->LoadModelFromFile( pModelPath );
			m_pRenderEntity[i]->SetScale( g_ModelList[i].Scale );
			m_pRenderEntity[i]->SetRotation( 0.0f, 0.0f, 0.0f );
			m_pRenderEntity[i]->SetPosition( g_ModelList[i].Position[0], g_ModelList[i].Position[1], g_ModelList[i].Position[2] );
			m_pRenderEntity[i]->SetMaterial( g_ModelList[i].MaterialName );
			m_pRenderEntity[i]->SetMaterialData( material );
			m_pRenderEntity[i]->SetConstantBuffer( ConstantTable::Instance()->CreateConstantBuffer( "ObjectCB" ) );
			m_uiModelCount++;
		}

		m_pCamera = new Camera();
		m_light   = new Light();

#if defined(SPONZA)
		m_pCamera->SetPosition( 180.0f, 180.0f, 0.0f );
		m_pCamera->SetTarget( 0.0f, 180.0f, 0.0f );
		m_light->Position = XMFLOAT3( 78.0f, 43.0f, 0.0f );
#endif

#if defined(CUBES)
		m_pCamera->SetPosition(0.0f, 5.0f, 5.0f);
		m_pCamera->SetTarget(0.0f, 0.0f, 0.0f);

		m_Light->Position = XMFLOAT3(-0.265f, 0.265f, -1.053f);
#endif

		m_pCamera->SetUp( 0.0f, 1.0f, 0.0f );
		m_pCamera->SetFieldOfView( 45.0f );
		m_pCamera->SetAspectRatio( 1920.0f / 1080.0f );

		m_light->Diffuse       = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		m_light->Ambient       = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		m_light->Specular      = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
		m_light->SpecularPower = 0.0f;

		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY )->ExecuteCommandLists();
		CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY )->Flush();

		return true;
	}

	void Renderer::Update( double _deltaTime )
	{
		UpdatePassConstants();

		for ( UINT i = 0; i < m_uiModelCount; ++i )
		{
			m_pRenderEntity[i]->Update();

			Object obj;
			obj.World    = m_pRenderEntity[i]->GetWorld();
			obj.Material = m_pRenderEntity[i]->GetMaterialData();

			if ( m_pRenderEntity[i]->GetConstantBuffer() )
				m_pRenderEntity[i]->GetConstantBuffer()->UpdateValue( "World", &obj, sizeof( Object ) );
		}
	}

	void Renderer::UpdatePassConstants() const
	{
		m_pCamera->Update();

		Pass cbPass           = Pass();
		cbPass.EyePosition    = m_pCamera->GetPosition();
		cbPass.ViewProjection = m_pCamera->GetView() * m_pCamera->GetProjection();

		if ( m_pMainPassCb )
			m_pMainPassCb->UpdateValue( nullptr, &cbPass, sizeof( Pass ) );

		if ( m_pLightsCb )
			m_pLightsCb->UpdateValue( nullptr, m_light, sizeof( Light ) );

		if ( m_pSpotlightCb )
			m_pSpotlightCb->UpdateValue( nullptr, m_spotlight, sizeof( Spotlight ) );
	}

	bool Renderer::Render( void )
	{
		DeviceD3D12::Instance()->BeginFrame();

		CommandList* pGfxCmdList = DeviceD3D12::Instance()->GetImmediateContext();
		pGfxCmdList->Reset();

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
			m_pSwapChain->Present();
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
			float v[3];
			v[0] = m_pCamera->GetPosition().x;
			v[1] = m_pCamera->GetPosition().y;
			v[2] = m_pCamera->GetPosition().z;

			if ( ImGui::SliderFloat3( "Position:", v, -1000.0f, 1000.0f ) )
			{
				m_pCamera->SetPosition( v[0], v[1], v[2] );
			}

			v[0] = m_pCamera->GetTarget().x;
			v[1] = m_pCamera->GetTarget().y;
			v[2] = m_pCamera->GetTarget().z;
			if ( ImGui::SliderFloat3( "Target:", v, -1000.0f, 1000.0f ) )
			{
				m_pCamera->SetTarget( v[0], v[1], v[2] );
			}

			v[0] = m_pCamera->GetUp().x;
			v[1] = m_pCamera->GetUp().y;
			v[2] = m_pCamera->GetUp().z;
			if ( ImGui::SliderFloat3( "Up:", v, -1000.0f, 1000.0f ) )
			{
				m_pCamera->SetUp( v[0], v[1], v[2] );
			}
			ImGui::End();
		}

		if ( ImGui::Begin( "Memory" ) )
		{
			static constexpr float kMB               = 1024 * 1024;
			float                  fTotalMemUsage    = 0.0f;
			int                    iTotalAllocations = 0;
			for ( unsigned int i = 0; i < static_cast<unsigned int>(MemoryContextCategory::ECategories); ++i )
			{
				const char*              pCatName = MemoryGlobalTracking::GetContextName( static_cast<MemoryContextCategory>(i) );
				const MemoryContextData& data     = MemoryGlobalTracking::GetContextStats( static_cast<MemoryContextCategory>(i) );

				const int iNetAllocations = data.Allocations - data.Deallocations;
				ImGui::CollapsingHeader( pCatName, ImGuiTreeNodeFlags_Bullet );
				ImGui::Text( "Net Allocations: %i", iNetAllocations );
				ImGui::Text( "Allocated Size: %0.3f MB", data.TotalAllocationSize / kMB );

				fTotalMemUsage += data.TotalAllocationSize;
				iTotalAllocations += iNetAllocations;
			}
			ImGui::CollapsingHeader( "Total Memory Usage:", ImGuiTreeNodeFlags_Bullet );
			ImGui::Text( "Size Allocation: %0.3f MB", fTotalMemUsage / kMB );
			ImGui::Text( "Net Allocations: %i", iTotalAllocations );
			ImGui::End();
		}

		if ( ImGui::Begin( "Lights" ) )
		{
			float v[3];
			v[0] = m_light->Position.x;
			v[1] = m_light->Position.y;
			v[2] = m_light->Position.z;
			if ( ImGui::SliderFloat3( "Position:", v, -180.0f, 180.0f ) )
			{
				m_light->Position.x = v[0];
				m_light->Position.y = v[1];
				m_light->Position.z = v[2];
			}

			v[0] = m_light->Diffuse.x;
			v[1] = m_light->Diffuse.y;
			v[2] = m_light->Diffuse.z;
			if ( ImGui::SliderFloat3( "Diffuse:", v, 0.0f, 1.0f ) )
			{
				m_light->Diffuse.x = v[0];
				m_light->Diffuse.y = v[1];
				m_light->Diffuse.z = v[2];
			}

			v[0] = m_light->Ambient.x;
			v[1] = m_light->Ambient.y;
			v[2] = m_light->Ambient.z;
			if ( ImGui::SliderFloat3( "Ambient:", v, 0.0f, 1.0f ) )
			{
				m_light->Ambient.x = v[0];
				m_light->Ambient.y = v[1];
				m_light->Ambient.z = v[2];
			}

			v[0] = m_light->Specular.x;
			v[1] = m_light->Specular.y;
			v[2] = m_light->Specular.z;
			if ( ImGui::SliderFloat3( "Specular:", v, 0.0f, 1.0f ) )
			{
				m_light->Specular.x = v[0];
				m_light->Specular.y = v[1];
				m_light->Specular.z = v[2];
			}

			float nS = m_light->SpecularPower;
			if ( ImGui::SliderFloat( "Specular Power:", &nS, 0.0f, 10.0f ) )
			{
				m_light->SpecularPower = nS;
			}

			ImGui::End();
		}

		if ( ImGui::Begin( "Objects:" ) )
		{
			float v[3];
			for ( unsigned int i = 0; i < m_uiModelCount; ++i )
			{
				if ( ImGui::CollapsingHeader( g_ModelList[i].ObjectName ) )
				{
					Material material = m_pRenderEntity[i]->GetMaterialData();

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

					m_pRenderEntity[i]->SetMaterialData( material );
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

		DeviceD3D12*   pDevice        = DeviceD3D12::Instance();
		ConstantTable* pConstantTable = ConstantTable::Instance();

		// Per Object Draws
		for ( UINT i = 0; i < m_uiModelCount; ++i )
		{
			const RenderEntity*     pModel   = m_pRenderEntity[i];
			DirectX::XMMATRIX       world    = pModel->GetWorld();
			ConstantBufferResource* pModelCb = pModel->GetConstantBuffer();

			pDevice->SetMaterial( pModel->GetMaterialName() );
			pDevice->SetSamplerState( "Albedo", pDevice->GetDefaultSamplerState() );
			pDevice->SetSamplerState( "Normal", pDevice->GetDefaultSamplerState() );

			pDevice->SetConstantBuffer( "ObjectCB", pModelCb );
			pDevice->SetConstantBuffer( "PassCB", m_pMainPassCb );
			pDevice->SetConstantBuffer( "LightCB", m_pLightsCb );
			pDevice->SetConstantBuffer( "SpotlightCB", m_pSpotlightCb );

			for ( UINT i = 0; i < pModel->GetModel()->MeshCount; ++i )
			{
				const Mesh& rMesh = pModel->GetModel()->pMeshList[i];

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
