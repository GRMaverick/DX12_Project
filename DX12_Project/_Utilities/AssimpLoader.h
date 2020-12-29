#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

#include "D3D12\Resources\IndexBufferResource.h"
#include "D3D12\Resources\VertexBufferResource.h"

#include <assimp\types.h>
#include <assimp\material.h>
#include <map>
#include <string>

class DeviceD3D12;
class CommandList;
struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;

struct Mesh
{
	UINT				Indices = 0;
	IBufferResource*	pTexture = nullptr;
	IBufferResource*	pIndexBuffer = nullptr;
	IBufferResource*	pVertexBuffer = nullptr;
};

struct RenderModel
{
	UINT				MeshCount = 0;
	Mesh*				pMeshList = nullptr;
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
	static bool						LoadModel(DeviceD3D12* _pDevice, CommandList* _pCommandList, const char* _pFilename, RenderModel** _pModelOut);
	static void						ProcessNode(DeviceD3D12* _pDevice, CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, RenderModel** _pModelOut);
	static Mesh						ProcessMesh(DeviceD3D12* _pDevice, CommandList* _pCommandList, const aiMesh* _pMesh, const aiScene* _pScene);
	static IBufferResource*			ProcessMaterial(DeviceD3D12* _pDevice, CommandList* _pCommandList, const aiMaterial* _pMaterial, const aiTextureType _type, const char* _typeName, const aiScene* _pScene);
private:
	static std::map<std::string, RenderModel*>		m_LoadedModels;
	static std::map<std::string, IBufferResource*>	m_LoadedTextures;
	
	static AssimpPreprocessResult	Preprocess(const aiScene* _pScene);
};
#endif __AssimpLoader_h__