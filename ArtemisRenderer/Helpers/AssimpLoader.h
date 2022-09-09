#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

#include "Resources/IndexBuffer.h"
#include "Resources/VertexBuffer.h"

#include <map>
#include <string>
#include <algorithm>

#include <assimp\types.h>
#include <assimp\material.h>

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;

#define ALBEDO 0
#define NORMAL 1
#define METALLIC 2
#define AOCCLUSION 3
#define DISPLACEMENT 4

namespace ArtemisRenderer::Resources
{
	class IBufferResource;
}

namespace ArtemisRenderer::Device
{
	class RenderDevice;
	class CommandList;
}

namespace ArtemisRenderer::Helpers
{
	struct Mesh
	{
		UINT						Indices = 0;
		Resources::IBufferResource* pTexture[2];
		Resources::IBufferResource* pIndexBuffer = nullptr;
		Resources::IBufferResource* pVertexBuffer = nullptr;
	};

	struct RenderModel
	{
		UINT	MeshCount = 0;
		Mesh*	pMeshList = nullptr;
	};

	struct AssimpPreprocessResult
	{
		UINT MeshCount = 0;
		UINT MaterialCount = 0;
		UINT TextureCount = 0;

		UINT DiffuseCount = 0;
		UINT NormalCount = 0;
		UINT SpecularCount = 0;
		UINT RoughnessCount = 0;
		UINT MetalnessCount = 0;
		UINT AOCount = 0;
	};

	class AssimpLoader
	{
	public:
		static bool							LoadModel(Device::RenderDevice* _pDevice, Device::CommandList* _pCommandList, const char* _pFilename, RenderModel** _pModelOut);
		static void							ProcessNode(Device::RenderDevice* _pDevice, Device::CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, RenderModel** _pModelOut);
		static Mesh							ProcessMesh(Device::RenderDevice* _pDevice, Device::CommandList* _pCommandList, const aiMesh* _pMesh, const aiScene* _pScene);
		static Resources::IBufferResource*	ProcessMaterial(Device::RenderDevice* _pDevice, Device::CommandList* _pCommandList, const aiMaterial* _pMaterial, const aiTextureType _type, const aiScene* _pScene);
	private:
		static std::map<std::string, RenderModel*>		m_LoadedModels;
		static std::map<std::string, Resources::IBufferResource*>	m_LoadedTextures;

		static AssimpPreprocessResult	Preprocess(const aiScene* _pScene);
	};
}

#endif __AssimpLoader_h__