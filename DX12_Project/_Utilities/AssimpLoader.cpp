#include "Defines.h"
#include "DeviceD3D12.h"
#include "CommandList.h"
#include "AssimpLoader.h"

#include <assert.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <WICTextureLoader.h>

#define ALBEDO 0

#pragma comment(lib, "assimp-vc142-mtd.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "zlibstaticd.lib")
#pragma comment(lib, "IrrXMLd.lib")

std::map<std::string, Model*> AssimpLoader::m_LoadedModels = std::map<std::string, Model*>();
std::map<std::string, Texture2DResource*> AssimpLoader::m_LoadedTextures = std::map<std::string, Texture2DResource*>();

struct Vertex
{
	float x, y, z = 0.0f;
	float u, v = 0.0f;
};

std::string g_Directory;

void PreprocessMesh(const aiMesh* _pMesh, const aiScene* _pScene, AssimpPreprocessResult* _pPpResult)
{
	if (_pMesh->mMaterialIndex >= 0)
	{
		const aiMaterial* pMaterial = _pScene->mMaterials[_pMesh->mMaterialIndex];
		_pPpResult->DiffuseCount += pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		_pPpResult->NormalCount += pMaterial->GetTextureCount(aiTextureType_NORMALS);
		_pPpResult->SpecularCount += pMaterial->GetTextureCount(aiTextureType_SPECULAR);
		_pPpResult->RoughnessCount += pMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
		_pPpResult->MetalnessCount += pMaterial->GetTextureCount(aiTextureType_METALNESS);
		_pPpResult->AOCount += pMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
	}
}

void PreprocessNode(const aiNode* _pNode, const aiScene* _pScene, AssimpPreprocessResult* _pPpResult)
{
	for (UINT i = 0; i < _pNode->mNumMeshes; ++i)
	{
		const aiMesh* pMesh = _pScene->mMeshes[_pNode->mMeshes[i]];
		PreprocessMesh(pMesh, _pScene, _pPpResult);
	}

	for (UINT i = 0; i < _pNode->mNumChildren; ++i)
	{
		PreprocessNode(_pNode->mChildren[i], _pScene, _pPpResult);
	}
}

AssimpPreprocessResult AssimpLoader::Preprocess(const aiScene* _pScene)
{
	AssimpPreprocessResult result;
	PreprocessNode(_pScene->mRootNode, _pScene, &result);
	result.MeshCount = _pScene->mNumMeshes;
	result.MaterialCount = _pScene->mNumMaterials;
	result.TextureCount = result.DiffuseCount + result.NormalCount + result.SpecularCount + result.RoughnessCount + result.MetalnessCount + result.AOCount;

	return result;
}

bool AssimpLoader::LoadModel(DeviceD3D12* _pDevice, CommandList* _pCommandList, const char* _pFilename, Model** _pModelOut)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(_pFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if(!pScene)
	{
		return false;
	}

	std::string strFilename(_pFilename);
	g_Directory = strFilename.substr(0, strFilename.find_last_of('\\'));

	AssimpPreprocessResult result = Preprocess(pScene);

	(*_pModelOut) = new Model();
	(*_pModelOut)->pMeshList = new Mesh[result.MeshCount];
	if (!_pDevice->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &(*_pModelOut)->pSRVHeap, result.TextureCount, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, L"AssimpLoader"))
		return false;

	ProcessNode(_pDevice, _pCommandList, pScene->mRootNode, pScene, _pModelOut);

	return true;
}

void AssimpLoader::ProcessNode(DeviceD3D12* _pDevice, CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, Model** _pModelOut)
{
	for (UINT i = 0; i < _pNode->mNumMeshes; ++i)
	{
		const aiMesh* pMesh = _pScene->mMeshes[_pNode->mMeshes[i]];
		(*_pModelOut)->pMeshList[(*_pModelOut)->MeshCount] = ProcessMesh(_pDevice, _pCommandList, (*_pModelOut)->pSRVHeap, pMesh, _pScene);
		(*_pModelOut)->MeshCount++;
	}

	for (UINT i = 0; i < _pNode->mNumChildren; ++i)
	{
		ProcessNode(_pDevice, _pCommandList, _pNode->mChildren[i], _pScene, _pModelOut);
	}
}

Mesh AssimpLoader::ProcessMesh(DeviceD3D12* _pDevice, CommandList* _pCommandList, DescriptorHeap* pDescHeapSRV, const aiMesh* _pMesh, const aiScene* _pScene)
{
	//
	// Vertices
	//
	Vertex* pVertices = new Vertex[_pMesh->mNumVertices];
	for (UINT i = 0; i < _pMesh->mNumVertices; ++i)
	{
		pVertices[i].x = (float)_pMesh->mVertices[i].x;
		pVertices[i].y = (float)_pMesh->mVertices[i].y;
		pVertices[i].z = (float)_pMesh->mVertices[i].z;

		if (_pMesh->mTextureCoords[ALBEDO])
		{
			pVertices[i].u = (float)_pMesh->mTextureCoords[ALBEDO][i].x;
			pVertices[i].v = (float)_pMesh->mTextureCoords[ALBEDO][i].y;
		}
	}

	//
	// Indices
	//
	std::vector<DWORD> vIndices;
	for (UINT i = 0; i < _pMesh->mNumFaces; ++i)
	{
		const aiFace face = _pMesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; ++j)
		{
			vIndices.push_back(face.mIndices[j]);
		}
	}

	//
	// Textures / Materials
	//
	Mesh mesh;
	if (_pMesh->mMaterialIndex >= 0)
	{
		const aiMaterial* pMaterial = _pScene->mMaterials[_pMesh->mMaterialIndex];
		mesh.pTexture = ProcessMaterial(_pDevice, _pCommandList, pDescHeapSRV, pMaterial, aiTextureType_DIFFUSE, "texture_diffuse", _pScene);
	}

	//
	// Upload GPU resources
	//
	mesh.Indices = (UINT32)vIndices.size();
	if (!_pDevice->CreateVertexBufferResource(_pCommandList, _pMesh->mNumVertices, sizeof(Vertex), D3D12_RESOURCE_FLAG_NONE, (void*)pVertices, &mesh.pVertexBuffer))
	{
		assert(false && "AssimpLoader: Could not load Vertex Buffer Resource");
	}
	if (!_pDevice->CreateIndexBufferResource(_pCommandList, (UINT)vIndices.size(), sizeof(DWORD), D3D12_RESOURCE_FLAG_NONE, (void*)&vIndices[0], &mesh.pIndexBuffer))
	{
		assert(false && "AssimpLoad: Could not load Index Buffer Resource");
	}
	return mesh;
}

std::string DetermineTextureType(const aiScene* scene, const aiMaterial* mat)
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	std::string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
	{
		if (scene->mTextures[0]->mHeight == 0)
		{
			return "embedded compressed texture";
		}
		else
		{
			return "embedded non-compressed texture";
		}
	}
	if (textypeteststr.find('.') != std::string::npos)
	{
		return "textures are on disk";
	}
	return "";
}

int GetTextureIndex(aiString* str)
{
	std::string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

Texture2DResource* GetTextureFromModel(DeviceD3D12* _pDevice, const aiScene* scene, int textureindex)
{
	PRAGMA_TODO("AssimpLoader WICTexturesFromMemory");

	return nullptr;
}

Texture2DResource* AssimpLoader::ProcessMaterial(DeviceD3D12* _pDevice, CommandList* _pCommandList, DescriptorHeap* pDescHeapSRV, const aiMaterial* _pMaterial, const aiTextureType _type, const char* _typeName, const aiScene* _pScene)
{
	assert((_pMaterial->GetTextureCount(_type) == 1) && "More than one textures of this type");

	for (UINT i = 0; i < _pMaterial->GetTextureCount(_type); ++i)
	{
		aiString str;
		_pMaterial->GetTexture(_type, i, &str);

		bool bSkip = m_LoadedTextures.find(str.C_Str()) != m_LoadedTextures.end();
		if (!bSkip)
		{
			Texture2DResource* pTexture;
			std::string textype = DetermineTextureType(_pScene, _pMaterial);
			if (textype == "embedded compressed texture")
			{
				//pTexture = GetTextureFromModel(_pDevice, _pScene, GetTextureIndex(&str));
			}
			else
			{
				std::string strFilename = g_Directory + "\\" +  std::string(str.C_Str());
				std::wstring wstrFilename = std::wstring(strFilename.begin(), strFilename.end());
				if (_pDevice->CreateWICTexture2D(wstrFilename.c_str(), _pCommandList, &pTexture, pDescHeapSRV, wstrFilename.c_str()))
					return pTexture;
			}
		}
	}
	return nullptr;
}
