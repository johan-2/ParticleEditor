#pragma once
#include "ModelComponent.h"
#include <vector>
#include <DirectXMath.h>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

class mesh;

using namespace DirectX;

struct ModelInstance
{
public:
	XMFLOAT4X4 worldMatrix;

	ModelInstance(XMFLOAT4X4 matrix) :
		worldMatrix(matrix) {}
};

class InstancedModel
{
public:
	InstancedModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"", wchar_t* emissiveMap = L"", bool useMaterial = true, float tiling = 1.0f, float heightMapScale = 0.04f);
	~InstancedModel();

	// get mesh list and how many meshes this model have
	const std::vector<Mesh*>& GetMeshes()       { return _meshes; }
	const unsigned int&       GetNumMeshes()    { return _numMeshes; }
	const unsigned int&       GetNumInstances() { return _numInstances; }
	unsigned int* GetStrides()                  { return _strides; }
	unsigned int* GetOffsets()                  { return _offsets; }

	// build the instance buffer from a list of world matrices
	void BuildInstanceBuffer(std::vector<ModelInstance>& instances);

	// render all instances of this model
	void UploadInstances();

private:

	void ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling, float heightMapScale);
	void AddToRenderQueues(bool add);

	// meshes list and num meshes count
	std::vector<Mesh*> _meshes;
	unsigned int       _numMeshes;
	unsigned int	   _numInstances;
	unsigned int       _strides[2];
	unsigned int       _offsets[2];

	ID3D11Buffer* _instanceBuffer;

	bool _useMaterial;

	// render flags
	unsigned int _FLAGS;
};

