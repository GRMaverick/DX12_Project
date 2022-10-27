#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\VertexBufferResource.h"

#include <assimp\types.h>
#include <assimp\material.h>
#include <map>
#include <string>

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;

#define ALBEDO 0
#define NORMAL 1
#define METALLIC 2
#define AOCCLUSION 3
#define DISPLACEMENT 4

namespace SysRenderer
{
	namespace D3D12
	{
		class DeviceD3D12;
		class CommandList;
	}

	namespace Loading
	{
		struct Mesh
		{
			UINT                         Indices = 0;
			Interfaces::IBufferResource* pTexture[2];
			Interfaces::IBufferResource* pIndexBuffer  = nullptr;
			Interfaces::IBufferResource* pVertexBuffer = nullptr;
		};

		struct RenderModel
		{
			UINT  MeshCount = 0;
			Mesh* pMeshList = nullptr;
		};

		struct AssimpPreprocessResult
		{
			UINT MeshCount     = 0;
			UINT MaterialCount = 0;
			UINT TextureCount  = 0;

			UINT DiffuseCount   = 0;
			UINT NormalCount    = 0;
			UINT SpecularCount  = 0;
			UINT RoughnessCount = 0;
			UINT MetalnessCount = 0;
			UINT AOCount        = 0;
		};

		class AssimpLoader
		{
		public:
			static bool                         LoadModel( D3D12::DeviceD3D12* _pDevice, D3D12::CommandList* _pCommandList, const char* _pFilename, RenderModel** _pModelOut );
			static void                         ProcessNode( D3D12::DeviceD3D12* _pDevice, D3D12::CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, RenderModel** _pModelOut );
			static Mesh                         ProcessMesh( D3D12::DeviceD3D12* _pDevice, D3D12::CommandList* _pCommandList, const aiMesh* _pMesh, const aiScene* _pScene );
			static Interfaces::IBufferResource* ProcessMaterial( D3D12::DeviceD3D12* _pDevice, D3D12::CommandList* _pCommandList, const aiMaterial* _pMaterial, const aiTextureType _type, const aiScene* _pScene );
		private:
			static std::map<std::string, RenderModel*>                 m_mapLoadedModels;
			static std::map<std::string, Interfaces::IBufferResource*> m_mapLoadedTextures;

			static AssimpPreprocessResult Preprocess( const aiScene* _pScene );
		};
	}
}

#endif __AssimpLoader_h__