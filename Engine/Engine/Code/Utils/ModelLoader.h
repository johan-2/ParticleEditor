#pragma once
#include <vector>
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class ModelLoader
{
public:
	static Mesh* CreateMesh(aiMesh* mesh, const aiScene* scene, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, float tiling, bool useMaterial, Entity* parent)
	{
		// structures for vertex/index data
		std::vector<Mesh::VertexData> vertices;
		std::vector<unsigned long>    indices;

		// set default textures, these will be used if useMaterial is set to false
		std::wstring diffuse  = diffuseMap;
		std::wstring normal   = normalMap;
		std::wstring specular = specularMap;

		// if we want to load the textures from a material file
		if (useMaterial)
		{
			// do this mesh have a material
			if (mesh->mMaterialIndex >= 0)
			{
				// get the material of this mesh
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				// diffuse map
				if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					aiString stringDiffuse;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &stringDiffuse);

					diffuse = GetRelativePathAndSetExtension(stringDiffuse.C_Str(), ".dds");
				}

				// normal map
				if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
				{
					aiString stringNormal;
					material->GetTexture(aiTextureType_HEIGHT, 0, &stringNormal);

					normal = GetRelativePathAndSetExtension(stringNormal.C_Str(), ".dds");
				}

				// specular map
				if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
				{
					aiString stringSpecular;
					material->GetTexture(aiTextureType_SPECULAR, 0, &stringSpecular);

					specular = GetRelativePathAndSetExtension(stringSpecular.C_Str(), ".dds");
				}
			}
		}

		// loop over all vertices of this mesh
		for (UINT i = 0; i < mesh->mNumVertices; i++)
		{
			// vertex data struct
			Mesh::VertexData vertex;

			// position
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;

			// tex coords
			if (mesh->mTextureCoords[0])
			{
				vertex.texture.x = (float)mesh->mTextureCoords[0][i].x * tiling;
				vertex.texture.y = (float)mesh->mTextureCoords[0][i].y * tiling;
			}

			// normals
			if (mesh->HasNormals())
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			// tangents and binormals
			if (mesh->HasTangentsAndBitangents())
			{
				vertex.tangent.x = mesh->mTangents[i].x;
				vertex.tangent.y = mesh->mTangents[i].y;
				vertex.tangent.z = mesh->mTangents[i].z;

				vertex.binormal.x = mesh->mBitangents[i].x;
				vertex.binormal.y = mesh->mBitangents[i].y;
				vertex.binormal.z = mesh->mBitangents[i].z;
			}

			// vertex colors
			if (mesh->HasVertexColors(0))
			{
				vertex.color.r = (char)mesh->mColors[0][i].r;
				vertex.color.g = (char)mesh->mColors[0][i].g;
				vertex.color.b = (char)mesh->mColors[0][i].b;
				vertex.color.a = (char)mesh->mColors[0][i].a;
			}
			else // if mesh don't have veretx colors set them to white
			{
				vertex.color.r = 255;
				vertex.color.g = 255;
				vertex.color.b = 255;
				vertex.color.a = 255;
			}

			vertices.push_back(vertex);
		}

		// loop over all faces and get the indices
		for (UINT i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (UINT j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// create mesh and buffers
		Mesh* modelMesh = new Mesh(parent, flags, diffuse.c_str(), normal.c_str(), specular.c_str());
		modelMesh->CreateBuffers(&vertices[0], &indices[0], vertices.size(), indices.size());

		return modelMesh;
	}

	static Mesh* CreateCube(unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, float tiling, Entity* parent)
	{
		// allocate memory for vertex and index buffers
		Mesh::VertexData vertices[24];
		unsigned long indices[36]
		{
			0,1,2,2,1,3,
			4,5,6,6,5,7,
			8,9,10,10,9,11,
			12,13,14,14,13,15,
			16,17,18,18,17,19,
			20,21,22,22,21,23
		};

		vertices[0].position = XMFLOAT3(-0.5, 0.5, -0.5);
		vertices[0].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[0].normal   = XMFLOAT3(0, 0, -1);
		vertices[0].tangent  = XMFLOAT3(1, 0, 0);
		vertices[0].binormal = XMFLOAT3(-0, 1, 0);
		vertices[0].color    = Color32(255, 255, 255, 255);

		vertices[1].position = XMFLOAT3(0.5, 0.5, -0.5);
		vertices[1].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[1].normal   = XMFLOAT3(0, 0, -1);
		vertices[1].tangent  = XMFLOAT3(1, 0, 0);
		vertices[1].binormal = XMFLOAT3(-0, 1, 0);
		vertices[1].color    = Color32(255, 255, 255, 255);

		vertices[2].position = XMFLOAT3(-0.5, -0.5, -0.5);
		vertices[2].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[2].normal   = XMFLOAT3(0, 0, -1);
		vertices[2].tangent  = XMFLOAT3(1, 0, 0);
		vertices[2].binormal = XMFLOAT3(-0, 1, 0);
		vertices[2].color    = Color32(255, 255, 255, 255);

		vertices[3].position = XMFLOAT3(0.5, -0.5, -0.5);
		vertices[3].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[3].normal   = XMFLOAT3(0, 0, -1);
		vertices[3].tangent  = XMFLOAT3(1, 0, 0);
		vertices[3].binormal = XMFLOAT3(-0, 1, 0);
		vertices[3].color    = Color32(255, 255, 255, 255);

		vertices[4].position = XMFLOAT3(0.5, 0.5, -0.5);
		vertices[4].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[4].normal   = XMFLOAT3(1, 0, 0);
		vertices[4].tangent  = XMFLOAT3(0, 0, 1);
		vertices[4].binormal = XMFLOAT3(0, 1, 0);
		vertices[4].color    = Color32(255, 255, 255, 255);

		vertices[5].position = XMFLOAT3(0.5, 0.5, 0.5);
		vertices[5].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[5].normal   = XMFLOAT3(1, 0, 0);
		vertices[5].tangent  = XMFLOAT3(0, 0, 1);
		vertices[5].binormal = XMFLOAT3(0, 1, 0);
		vertices[5].color    = Color32(255, 255, 255, 255);

		vertices[6].position = XMFLOAT3(0.5, -0.5, -0.5);
		vertices[6].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[6].normal   = XMFLOAT3(1, 0, 0);
		vertices[6].tangent  = XMFLOAT3(0, 0, 1);
		vertices[6].binormal = XMFLOAT3(0, 1, 0);
		vertices[6].color    = Color32(255, 255, 255, 255);

		vertices[7].position = XMFLOAT3(0.5, -0.5, 0.5);
		vertices[7].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[7].normal   = XMFLOAT3(1, 0, 0);
		vertices[7].tangent  = XMFLOAT3(0, 0, 1);
		vertices[7].binormal = XMFLOAT3(0, 1, 0);
		vertices[7].color    = Color32(255, 255, 255, 255);

		vertices[8].position = XMFLOAT3(0.5, 0.5, 0.5);
		vertices[8].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[8].normal   = XMFLOAT3(0, 0, 1);
		vertices[8].tangent  = XMFLOAT3(-1, 0, 0);
		vertices[8].binormal = XMFLOAT3(0, 1, -0);
		vertices[8].color    = Color32(255, 255, 255, 255);

		vertices[9].position = XMFLOAT3(-0.5, 0.5, 0.5);
		vertices[9].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[9].normal   = XMFLOAT3(0, 0, 1);
		vertices[9].tangent  = XMFLOAT3(-1, 0, 0);
		vertices[9].binormal = XMFLOAT3(0, 1, -0);
		vertices[9].color    = Color32(255, 255, 255, 255);

		vertices[10].position = XMFLOAT3(0.5, -0.5, 0.5);
		vertices[10].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[10].normal   = XMFLOAT3(0, 0, 1);
		vertices[10].tangent  = XMFLOAT3(-1, 0, 0);
		vertices[10].binormal = XMFLOAT3(0, 1, -0);
		vertices[10].color    = Color32(255, 255, 255, 255);

		vertices[11].position = XMFLOAT3(-0.5, -0.5, 0.5);
		vertices[11].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[11].normal   = XMFLOAT3(0, 0, 1);
		vertices[11].tangent  = XMFLOAT3(-1, 0, 0);
		vertices[11].binormal = XMFLOAT3(0, 1, -0);
		vertices[11].color    = Color32(255, 255, 255, 255);

		vertices[12].position = XMFLOAT3(-0.5, 0.5, 0.5);
		vertices[12].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[12].normal   = XMFLOAT3(-1, 0, 0);
		vertices[12].tangent  = XMFLOAT3(0, 0, -1);
		vertices[12].binormal = XMFLOAT3(0, 1, 0);
		vertices[12].color    = Color32(255, 255, 255, 255);

		vertices[13].position = XMFLOAT3(-0.5, 0.5, -0.5);
		vertices[13].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[13].normal   = XMFLOAT3(-1, 0, 0);
		vertices[13].tangent  = XMFLOAT3(0, 0, -1);
		vertices[13].binormal = XMFLOAT3(0, 1, 0);
		vertices[13].color    = Color32(255, 255, 255, 255);

		vertices[14].position = XMFLOAT3(-0.5, -0.5, 0.5);
		vertices[14].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[14].normal   = XMFLOAT3(-1, 0, 0);
		vertices[14].tangent  = XMFLOAT3(0, 0, -1);
		vertices[14].binormal = XMFLOAT3(0, 1, 0);
		vertices[14].color    = Color32(255, 255, 255, 255);

		vertices[15].position = XMFLOAT3(-0.5, -0.5, -0.5);
		vertices[15].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[15].normal   = XMFLOAT3(-1, 0, 0);
		vertices[15].tangent  = XMFLOAT3(0, 0, -1);
		vertices[15].binormal = XMFLOAT3(0, 1, 0);
		vertices[15].color    = Color32(255, 255, 255, 255);

		vertices[16].position = XMFLOAT3(-0.5, 0.5, 0.5);
		vertices[16].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[16].normal   = XMFLOAT3(0, 1, 0);
		vertices[16].tangent  = XMFLOAT3(1, 0, 0);
		vertices[16].binormal = XMFLOAT3(0, 0, 1);
		vertices[16].color    = Color32(255, 255, 255, 255);

		vertices[17].position = XMFLOAT3(0.5, 0.5, 0.5);
		vertices[17].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[17].normal   = XMFLOAT3(0, 1, 0);
		vertices[17].tangent  = XMFLOAT3(1, 0, 0);
		vertices[17].binormal = XMFLOAT3(0, 0, 1);
		vertices[17].color    = Color32(255, 255, 255, 255);

		vertices[18].position = XMFLOAT3(-0.5, 0.5, -0.5);
		vertices[18].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[18].normal   = XMFLOAT3(0, 1, 0);
		vertices[18].tangent  = XMFLOAT3(1, 0, 0);
		vertices[18].binormal = XMFLOAT3(0, 0, 1);
		vertices[18].color    = Color32(255, 255, 255, 255);

		vertices[19].position = XMFLOAT3(0.5, 0.5, -0.5);
		vertices[19].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[19].normal   = XMFLOAT3(0, 1, 0);
		vertices[19].tangent  = XMFLOAT3(1, 0, 0);
		vertices[19].binormal = XMFLOAT3(0, 0, 1);
		vertices[19].color    = Color32(255, 255, 255, 255);

		vertices[20].position = XMFLOAT3(-0.5, -0.5, -0.5);
		vertices[20].texture  = XMFLOAT2(0 * tiling, 0 * tiling);
		vertices[20].normal   = XMFLOAT3(0, -1, 0);
		vertices[20].tangent  = XMFLOAT3(1, 0, 0);
		vertices[20].binormal = XMFLOAT3(0, 0, -1);
		vertices[20].color    = Color32(255, 255, 255, 255);

		vertices[21].position = XMFLOAT3(0.5, -0.5, -0.5);
		vertices[21].texture  = XMFLOAT2(1 * tiling, 0 * tiling);
		vertices[21].normal   = XMFLOAT3(0, -1, 0);
		vertices[21].tangent  = XMFLOAT3(1, 0, 0);
		vertices[21].binormal = XMFLOAT3(0, 0, -1);
		vertices[21].color    = Color32(255, 255, 255, 255);

		vertices[22].position = XMFLOAT3(-0.5, -0.5, 0.5);
		vertices[22].texture  = XMFLOAT2(0 * tiling, 1 * tiling);
		vertices[22].normal   = XMFLOAT3(0, -1, 0);
		vertices[22].tangent  = XMFLOAT3(1, 0, 0);
		vertices[22].binormal = XMFLOAT3(0, 0, -1);
		vertices[22].color    = Color32(255, 255, 255, 255);

		vertices[23].position = XMFLOAT3(0.5, -0.5, 0.5);
		vertices[23].texture  = XMFLOAT2(1 * tiling, 1 * tiling);
		vertices[23].normal   = XMFLOAT3(0, -1, 0);
		vertices[23].tangent  = XMFLOAT3(1, 0, 0);
		vertices[23].binormal = XMFLOAT3(0, 0, -1);
		vertices[23].color    = Color32(255, 255, 255, 255);

		// create mesh and buffers	
		Mesh* mesh = new Mesh(parent, flags, diffuseMap, normalMap, specularMap);
		mesh->CreateBuffers(vertices, indices, 24, 36);

		return mesh;
	}

	static Mesh* CreatePlane(unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, float tiling, Entity* parent)
	{
		Mesh::VertexData vertices[4];

		vertices[0].position = XMFLOAT3(-2.0f, 0.0f, 2.0f);
		vertices[0].texture  = XMFLOAT2(0.0f * tiling, 0.0f * tiling);
		vertices[0].normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[0].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertices[0].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices[0].color    = Color32(255, 255, 255, 255);

		vertices[1].position = XMFLOAT3(2.0f, 0.0f, 2.0f);
		vertices[1].texture  = XMFLOAT2(1.0f * tiling, 0.0f * tiling);
		vertices[1].normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[1].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertices[1].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices[1].color    = Color32(255, 255, 255, 255);

		vertices[2].position = XMFLOAT3(-2.0f, 0.0f, -2.0f);
		vertices[2].texture  = XMFLOAT2(0.0f * tiling, 1.0f * tiling);
		vertices[2].normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[2].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertices[2].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices[2].color    = Color32(255, 255, 255, 255);

		vertices[3].position = XMFLOAT3(2.0f, 0.0f, -2.0f);
		vertices[3].texture  = XMFLOAT2(1.0f * tiling, 1.0f * tiling);
		vertices[3].normal   = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertices[3].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertices[3].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertices[3].color    = Color32(255, 255, 255, 255);

		unsigned long indices[6]{ 0,1,2,2,1,3 };

		// create mesh and buffers	
		Mesh* mesh = new Mesh(parent, flags, diffuseMap, normalMap, specularMap);
		mesh->CreateBuffers(vertices, indices, 24, 36);

		return mesh;
	}	

	static Mesh* CreateGrid(unsigned int size, float cellSize, Color32 gridColor, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, float tiling, Entity* parent)
	{
		Mesh::VertexData* vertices = new Mesh::VertexData[4 * (size * size)];
		unsigned long*    indices  = new unsigned long[6 * (size * size)];

		float halfSize               = (size / 2.0f) * cellSize;
		unsigned int numQuads        = size * size;
		unsigned int quadsPerRow     = size;
		unsigned int horizontalCount = 0;
		unsigned int verticalCount   = 0;

		for (int i = 0; i < numQuads; i++)
		{
			// get position of all vertices
			XMFLOAT3 topLeft     = XMFLOAT3(-halfSize + (horizontalCount * cellSize), 0.0f, halfSize - (verticalCount * cellSize));
			XMFLOAT3 topRight    = XMFLOAT3(-halfSize + (horizontalCount * cellSize) + cellSize, 0.0f, halfSize - (verticalCount * cellSize));
			XMFLOAT3 bottomLeft  = XMFLOAT3(-halfSize + (horizontalCount * cellSize), 0.0f, halfSize - (verticalCount * cellSize) + cellSize);
			XMFLOAT3 bottomRight = XMFLOAT3(-halfSize + (horizontalCount * cellSize) + cellSize, 0.0f, halfSize - (verticalCount * cellSize) + cellSize);

			// set the vertex data
			vertices[(i * 4) + 0].position = topLeft; // top left
			vertices[(i * 4) + 0].texture  = XMFLOAT2(InverseLerp(-halfSize, halfSize, topLeft.x) * tiling, InverseLerp(-halfSize, halfSize, topLeft.z) * tiling);
			vertices[(i * 4) + 0].normal   = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertices[(i * 4) + 0].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertices[(i * 4) + 0].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertices[(i * 4) + 0].color    = gridColor;

			vertices[(i * 4) + 1].position = topRight;  // top right
			vertices[(i * 4) + 1].texture  = XMFLOAT2(InverseLerp(-halfSize, halfSize, topRight.x) * tiling, InverseLerp(-halfSize, halfSize, topRight.z) * tiling);
			vertices[(i * 4) + 1].normal   = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertices[(i * 4) + 1].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertices[(i * 4) + 1].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertices[(i * 4) + 1].color    = gridColor;

			vertices[(i * 4) + 2].position = bottomLeft;  // bottom left
			vertices[(i * 4) + 2].texture  = XMFLOAT2(InverseLerp(-halfSize, halfSize, bottomLeft.x) * tiling, InverseLerp(-halfSize, halfSize, bottomLeft.z) * tiling);
			vertices[(i * 4) + 2].normal   = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertices[(i * 4) + 2].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertices[(i * 4) + 2].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertices[(i * 4) + 2].color    = gridColor;

			vertices[(i * 4) + 3].position = bottomRight; // bottom right
			vertices[(i * 4) + 3].texture  = XMFLOAT2(InverseLerp(-halfSize, halfSize, bottomRight.x) * tiling, InverseLerp(-halfSize, halfSize, bottomRight.z) * tiling);
			vertices[(i * 4) + 3].normal   = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertices[(i * 4) + 3].tangent  = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertices[(i * 4) + 3].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertices[(i * 4) + 3].color    = gridColor;

			indices[(i * 6) + 0] = (i * 4) + 0;
			indices[(i * 6) + 1] = (i * 4) + 1;
			indices[(i * 6) + 2] = (i * 4) + 2;
			indices[(i * 6) + 3] = (i * 4) + 2;
			indices[(i * 6) + 4] = (i * 4) + 1;
			indices[(i * 6) + 5] = (i * 4) + 3;

			horizontalCount++;
			if (horizontalCount == quadsPerRow)
			{
				verticalCount++;
				horizontalCount = 0;
			}
		}

		// create mesh and buffers	
		Mesh* mesh = new Mesh(parent, flags, diffuseMap, normalMap, specularMap);
		mesh->CreateBuffers(vertices, indices, 4 * (size * size), 6 * (size * size));

		delete[] vertices;
		delete[] indices;

		return mesh;
	}

	static std::wstring GetRelativePathAndSetExtension(const char* filePath, const char* extension)
	{
		std::string filename(filePath);

		// get the offset where our last backslash is located
		// we only want the name of the texture
		const size_t lastSlash = filename.find_last_of("\\");

		// erease filepath
		if (std::string::npos != lastSlash)
			filename.erase(0, lastSlash + 1);

		// change all texture exstensions to .dds so if the model
		// material was exported using other filetypes we don't have to change the .mtl file
		filename = filename.substr(0, filename.find_last_of('.')) + extension;

		// add our relative path to our textures
		std::string relativeFilePath = "Textures/";
		relativeFilePath.append(filename.c_str());

		// conert to wide string (only supports asciII characters)
		std::wstring wtp(relativeFilePath.begin(), relativeFilePath.end());

		return wtp;
	}

	static float InverseLerp(float a, float b, float t)
	{
		return (t - a) / (b - a);
	}
};

	
