#pragma once
#include "ModelComponent.h"
#include <vector>
#include <DirectXMath.h>

#include "ThirdParty/assimp/Importer.hpp"
#include "ThirdParty/assimp/scene.h"
#include "ThirdParty/assimp/postprocess.h"

class mesh;

using namespace DirectX;

struct ModelInstance
{
	XMFLOAT4X4 worldMatrix;

	ModelInstance(XMFLOAT4X4 matrix) :
		worldMatrix(matrix) {}
};

class InstancedModel
{
public:
	InstancedModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"", wchar_t* emissiveMap = L"", bool useMaterial = true, float tiling = 1.0f, float heightMapScale = 0.04f);
	~InstancedModel();

	// build the instance buffer from a list of world matrices
	void BuildInstanceBuffer(std::vector<ModelInstance>& instances);

	// render all instances of this model
	void UploadInstances();

	// meshes data
	std::vector<Mesh*> meshes;
	unsigned int       numMeshes;
	unsigned int	   numInstances;
	unsigned int       strides[2];
	unsigned int       offsets[2];

private:

	void ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling, float heightMapScale);
	void AddToRenderQueues(bool add);

	ID3D11Buffer* _instanceBuffer;

	// render flags and if model have a .mtl file
	unsigned int _FLAGS;
	bool         _useMaterial;
};

