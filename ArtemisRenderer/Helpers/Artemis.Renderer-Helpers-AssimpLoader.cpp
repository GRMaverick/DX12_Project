module;

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <WICTextureLoader.h>

#include "AssimpLoader.h"

#define ALBEDO 0

#pragma comment(lib, "assimp-vc142-mtd.lib")
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "zlibstaticd.lib")
#pragma comment(lib, "IrrXMLd.lib")

module Artemis.Renderer:Helpers;

import "Defines.h";

import "Device/RenderDevice.h";
import "Device/CommandList.h";

import "Resources/Texture2D.h";

using namespace ArtemisRenderer::Device;
using namespace ArtemisRenderer::Resources;

namespace ArtemisRenderer::Helpers
{
	std::map<std::string, RenderModel*> AssimpLoader::m_LoadedModels = std::map<std::string, RenderModel*>();
	std::map<std::string, IBufferResource*> AssimpLoader::m_LoadedTextures = std::map<std::string, IBufferResource*>();

	struct Vertex
	{
		float vX, vY, vZ = 0.0f;
		float nX, nY, nZ = 0.0f;
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

	bool AssimpLoader::LoadModel(RenderDevice* _pDevice, CommandList* _pCommandList, const char* _pFilename, RenderModel** _pModelOut)
	{
		Assimp::Importer importer;

		const aiScene* pScene = importer.ReadFile(_pFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
		if (!pScene)
		{
			return false;
		}

		std::string strFilename(_pFilename);
		if (m_LoadedModels.find(strFilename) == m_LoadedModels.end())
		{
			g_Directory = strFilename.substr(0, strFilename.find_last_of('\\'));

			AssimpPreprocessResult result = Preprocess(pScene);

			(*_pModelOut) = new RenderModel();
			(*_pModelOut)->pMeshList = new Mesh[result.MeshCount];

			ProcessNode(_pDevice, _pCommandList, pScene->mRootNode, pScene, _pModelOut);
			m_LoadedModels[strFilename] = (*_pModelOut);
		}
		else
		{
			(*_pModelOut) = m_LoadedModels[strFilename];
		}
		return true;
	}

	void AssimpLoader::ProcessNode(RenderDevice* _pDevice, CommandList* _pCommandList, const aiNode* _pNode, const aiScene* _pScene, RenderModel** _pModelOut)
	{
		for (UINT i = 0; i < _pNode->mNumMeshes; ++i)
		{
			const aiMesh* pMesh = _pScene->mMeshes[_pNode->mMeshes[i]];
			(*_pModelOut)->pMeshList[(*_pModelOut)->MeshCount] = ProcessMesh(_pDevice, _pCommandList, pMesh, _pScene);
			(*_pModelOut)->MeshCount++;
		}

		for (UINT i = 0; i < _pNode->mNumChildren; ++i)
		{
			ProcessNode(_pDevice, _pCommandList, _pNode->mChildren[i], _pScene, _pModelOut);
		}
	}

	Mesh AssimpLoader::ProcessMesh(RenderDevice* _pDevice, CommandList* _pCommandList, const aiMesh* _pMesh, const aiScene* _pScene)
	{
		unsigned int stride = sizeof(float) * 3;

		unsigned int uiFullBufferLength = 0;
		float* pPositions = new float[_pMesh->mNumVertices * 3];
		for (unsigned int rhs = 0, lhs = 0; rhs < _pMesh->mNumVertices; ++rhs, lhs += 3)
		{
			pPositions[lhs] = _pMesh->mVertices[rhs].x;
			pPositions[lhs + 1] = _pMesh->mVertices[rhs].y;
			pPositions[lhs + 2] = _pMesh->mVertices[rhs].z;
			uiFullBufferLength += 3;
		}

		float* pNormals = nullptr;
		if (_pMesh->HasNormals())
		{
			stride += (sizeof(float) * 3);
			pNormals = new float[_pMesh->mNumVertices * 3];
			for (unsigned int rhs = 0, lhs = 0; rhs < _pMesh->mNumVertices; ++rhs, lhs += 3)
			{
				pNormals[lhs] = _pMesh->mNormals[rhs].x;
				pNormals[lhs + 1] = _pMesh->mNormals[rhs].y;
				pNormals[lhs + 2] = _pMesh->mNormals[rhs].z;
				uiFullBufferLength += 3;
			}
		}

		float* pTangents = nullptr;
		float* pBitangents = nullptr;

		float* pTextureCoords = nullptr;
		if (_pMesh->HasTextureCoords(ALBEDO))
		{
			stride += (sizeof(float) * 2);
			pTextureCoords = new float[_pMesh->mNumVertices * 2];
			for (unsigned int rhs = 0, lhs = 0; rhs < _pMesh->mNumVertices; ++rhs, lhs += 2)
			{
				pTextureCoords[lhs] = (float)_pMesh->mTextureCoords[ALBEDO][rhs].x;
				pTextureCoords[lhs + 1] = (float)_pMesh->mTextureCoords[ALBEDO][rhs].y;
				uiFullBufferLength += 2;
			}
		}


		//
		// Textures / Materials
		//
		Mesh mesh;
		if (_pMesh->mMaterialIndex >= 0)
		{
			const aiMaterial* pMaterial = _pScene->mMaterials[_pMesh->mMaterialIndex];
			mesh.pTexture[ALBEDO] = ProcessMaterial(_pDevice, _pCommandList, pMaterial, aiTextureType_DIFFUSE, _pScene);
			mesh.pTexture[NORMAL] = ProcessMaterial(_pDevice, _pCommandList, pMaterial, aiTextureType_NORMALS, _pScene);

			if (mesh.pTexture[NORMAL])
			{
				if (_pMesh->HasTangentsAndBitangents())
				{
					stride += sizeof(float) * 3 * 2; // Tangents and Bitangents

					pTangents = new float[_pMesh->mNumVertices * 3];
					pBitangents = new float[_pMesh->mNumVertices * 3];

					for (unsigned int lhs = 0, rhs = 0; rhs < _pMesh->mNumVertices; ++rhs, lhs += 3)
					{
						pTangents[lhs] = _pMesh->mTangents[rhs].x;
						pTangents[lhs + 1] = _pMesh->mTangents[rhs].y;
						pTangents[lhs + 2] = _pMesh->mTangents[rhs].z;
						uiFullBufferLength += 3;

						pBitangents[lhs] = _pMesh->mBitangents[rhs].x;
						pBitangents[lhs + 1] = _pMesh->mBitangents[rhs].y;
						pBitangents[lhs + 2] = _pMesh->mBitangents[rhs].z;
						uiFullBufferLength += 3;
					}
				}
			}
		}

		//
		// Indices
		//
		UINT32 iIndices = 0;
		for (UINT i = 0; i < _pMesh->mNumFaces; ++i)
		{
			const aiFace face = _pMesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j)
			{
				iIndices++;
			}
		}
		DWORD* pIndices = new DWORD[iIndices];
		for (UINT i = 0, index = 0; i < _pMesh->mNumFaces; ++i)
		{
			const aiFace face = _pMesh->mFaces[i];
			for (UINT j = 0; j < face.mNumIndices; ++j, ++index)
			{
				pIndices[index] = (DWORD)face.mIndices[j];
			}
		}

		assert((uiFullBufferLength * sizeof(float) / stride) == _pMesh->mNumVertices);

		//
		// Build Full Buffer
		//
		unsigned int bufferPos = 0;
		float* pFullBuffer = new float[uiFullBufferLength];
		for (unsigned int i = 0, triplePos = 0, doublePos = 0; i < uiFullBufferLength; i += bufferPos, triplePos += 3, doublePos += 2)
		{
			bufferPos = 0;
			if (pPositions)
			{
				pFullBuffer[i + bufferPos] = pPositions[triplePos];
				pFullBuffer[i + (bufferPos + 1)] = pPositions[triplePos + 1];
				pFullBuffer[i + (bufferPos + 2)] = pPositions[triplePos + 2];
				bufferPos += 3;
			}

			if (pNormals)
			{
				pFullBuffer[i + bufferPos] = pNormals[triplePos];
				pFullBuffer[i + (bufferPos + 1)] = pNormals[triplePos + 1];
				pFullBuffer[i + (bufferPos + 2)] = pNormals[triplePos + 2];
				bufferPos += 3;
			}

			if (pTextureCoords)
			{
				pFullBuffer[i + bufferPos] = pTextureCoords[doublePos];
				pFullBuffer[i + (bufferPos + 1)] = pTextureCoords[doublePos + 1];
				bufferPos += 2;
			}

			if (pTangents)
			{
				pFullBuffer[i + bufferPos] = pTangents[triplePos];
				pFullBuffer[i + (bufferPos + 1)] = pTangents[triplePos + 1];
				pFullBuffer[i + (bufferPos + 2)] = pTangents[triplePos + 2];
				bufferPos += 3;
			}

			if (pBitangents)
			{
				pFullBuffer[i + bufferPos] = pBitangents[triplePos];
				pFullBuffer[i + (bufferPos + 1)] = pBitangents[triplePos + 1];
				pFullBuffer[i + (bufferPos + 2)] = pBitangents[triplePos + 2];
				bufferPos += 3;
			}
		}

		//
		// Upload GPU resources
		//
		mesh.Indices = iIndices;
		mesh.pVertexBuffer = _pDevice->CreateVertexBufferResource(_pCommandList, uiFullBufferLength * sizeof(float), stride, D3D12_RESOURCE_FLAG_NONE, (void*)pFullBuffer);
		mesh.pIndexBuffer = _pDevice->CreateIndexBufferResource(_pCommandList, iIndices * sizeof(DWORD), sizeof(DWORD), D3D12_RESOURCE_FLAG_NONE, pIndices);

		if (pTangents) delete[] pTangents;
		if (pBitangents) delete[] pBitangents;
		if (pPositions) delete[] pPositions;
		if (pNormals) delete[] pNormals;
		if (pTextureCoords) delete[] pTextureCoords;

		delete[] pFullBuffer;
		delete[] pIndices;

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

	Texture2D* GetTextureFromModel(RenderDevice* _pDevice, const aiScene* scene, int textureindex)
	{
		PRAGMA_TODO("AssimpLoader WICTexturesFromMemory");

		return nullptr;
	}

	const char* GetExtension(const char* _pFilename)
	{
		size_t extensionStart = 0;
		size_t size = strlen(_pFilename);
		for (size_t i = size - 1; i >= 0; --i)
		{
			if (_pFilename[i] == '.')
			{
				extensionStart = i + 1;
				break;
			}
		}

		char* ext = new char[5];
		snprintf(ext, 5, "%s", &_pFilename[extensionStart]);
		return ext;
	}

	IBufferResource* AssimpLoader::ProcessMaterial(RenderDevice* _pDevice, CommandList* _pCommandList, const aiMaterial* _pMaterial, const aiTextureType _type, const aiScene* _pScene)
	{
		//assert((_pMaterial->GetTextureCount(_type) == 1) && "More than one textures of this type");

		for (UINT i = 0; i < _pMaterial->GetTextureCount(_type); ++i)
		{
			aiString str;
			_pMaterial->GetTexture(_type, i, &str);

			bool bSkip = m_LoadedTextures.find(str.C_Str()) != m_LoadedTextures.end();
			if (!bSkip)
			{
				IBufferResource* pTexture = nullptr;
				std::string textype = DetermineTextureType(_pScene, _pMaterial);
				if (textype == "embedded compressed texture")
				{
					//pTexture = GetTextureFromModel(_pDevice, _pScene, GetTextureIndex(&str));
				}
				else
				{
					std::string strFilename = g_Directory + "\\" + std::string(str.C_Str());
					std::wstring wstrFilename = std::wstring(strFilename.begin(), strFilename.end());
					const char* ext = GetExtension(str.C_Str());
					if (strncmp(ext, "dds", 3) == 0)
					{
						pTexture = _pDevice->CreateTexture2D(wstrFilename.c_str(), _pCommandList, wstrFilename.c_str());
					}
					else
					{
						pTexture = _pDevice->CreateWICTexture2D(wstrFilename.c_str(), _pCommandList, wstrFilename.c_str());
					}
					delete[] ext;
				}
				m_LoadedTextures[str.C_Str()] = pTexture;
				return pTexture;
			}
			else
			{
				return m_LoadedTextures[str.C_Str()];
			}
		}
		return nullptr;
	}
}