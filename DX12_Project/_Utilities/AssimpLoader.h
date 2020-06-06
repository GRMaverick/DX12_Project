#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

#include "IndexBufferResource.h"
#include "VertexBufferResource.h"

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
	UINT					Indices = 0;
	Texture2DResource*		pTexture = nullptr;
	IndexBufferResource*	pIndexBuffer = nullptr;
	VertexBufferResource*	pVertexBuffer = nullptr;
};

struct Model
{
	UINT				MeshCount = 0;
	Mesh*				pMeshList = nullptr;
	DescriptorHeap*		pSRVHeap = nullptr;
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
	static bool						LoadModel(DeviceD3D12* _pDevice, CommandList* _pCommandList, const char* _pFilename, Model** _pModelOut);
	static void						ProcessNode(DeviceD3D12* _pDevice, CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, Model** _pModelOut);
	static Mesh						ProcessMesh(DeviceD3D12* _pDevice, CommandList* _pCommandList, DescriptorHeap* pDescHeapSRV, const aiMesh* _pMesh, const aiScene* _pScene);
	static Texture2DResource*		ProcessMaterial(DeviceD3D12* _pDevice, CommandList* _pCommandList, DescriptorHeap* pDescHeapSRV, const aiMaterial* _pMaterial, const aiTextureType _type, const char* _typeName, const aiScene* _pScene);
private:
	static std::map<std::string, Model*>				m_LoadedModels;
	static std::map<std::string, Texture2DResource*>	m_LoadedTextures;
	
	static AssimpPreprocessResult	Preprocess(const aiScene* _pScene);
};
#endif __AssimpLoader_h__