#pragma once
#include "ModelComponent.h"
#include <vector>
#include <DirectXMath.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class mesh;

using namespace DirectX;

class InstancedModel
{
public:
	InstancedModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"", wchar_t* emissiveMap = L"", bool useMaterial = true, float tiling = 1.0f);
	~InstancedModel();

	// get mesh list and how many meshes this model have
	const std::vector<Mesh*>& GetMeshes() { return _meshes; }
	const unsigned int&       GetNumMeshes() { return _numMeshes; }

	void BuildInstanceBuffer();
	void AddInstance(XMFLOAT4X4 worldMatrix);
	void RenderInstances();

private:

	struct InstanceType
	{
	public:
		XMFLOAT4X4 worldMatrix;

		InstanceType(XMFLOAT4X4 matrix):
			worldMatrix(matrix){}
	};

	void ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling);
	void AddToRenderQueues(bool add);

	// meshes list and num meshes count
	std::vector<InstanceType> _instances;
	std::vector<Mesh*>        _meshes;
	unsigned int              _numMeshes;

	ID3D11Buffer* _instanceBuffer;

	bool _useMaterial;

	// render flags
	unsigned int _FLAGS;
};

