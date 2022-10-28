#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

//#include "Resources/IndexBufferResource.h"
//#include "Resources/VertexBufferResource.h"

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

namespace Artemis::Renderer::Interfaces
{
	class IGraphicsDevice;
	class ICommandList;
	class IGpuResource;
}

namespace Artemis::Renderer::Assets
{
	struct Mesh
	{
		unsigned int                           Indices = 0;
		Renderer::Interfaces::IGpuResource* pTexture[2];
		Renderer::Interfaces::IGpuResource* pIndexBuffer  = nullptr;
		Renderer::Interfaces::IGpuResource* pVertexBuffer = nullptr;
	};

	struct RenderModel
	{
		unsigned int MeshCount = 0;
		Mesh*        pMeshList = nullptr;
	};

	struct AssimpPreprocessResult
	{
		unsigned int MeshCount     = 0;
		unsigned int MaterialCount = 0;
		unsigned int TextureCount  = 0;

		unsigned int DiffuseCount   = 0;
		unsigned int NormalCount    = 0;
		unsigned int SpecularCount  = 0;
		unsigned int RoughnessCount = 0;
		unsigned int MetalnessCount = 0;
		unsigned int AOCount        = 0;
	};

	class AssimpLoader
	{
	public:
		static bool                         LoadModel( Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, const char* _pFilename, RenderModel** _pModelOut );
		static void                         ProcessNode( Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, RenderModel** _pModelOut );
		static Mesh                         ProcessMesh( Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, const aiMesh* _pMesh, const aiScene* _pScene );
		static Interfaces::IGpuResource* ProcessMaterial( Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCommandList, const aiMaterial* _pMaterial, const aiTextureType _type, const aiScene* _pScene );
	private:
		static std::map<std::string, RenderModel*>                 m_mapLoadedModels;
		static std::map<std::string, Interfaces::IGpuResource*> m_mapLoadedTextures;

		static AssimpPreprocessResult Preprocess( const aiScene* _pScene );
	};
}

#endif __AssimpLoader_h__
