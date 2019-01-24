#pragma once
#include <d3d11.h>
#include "IComponent.h"
#include <vector>
#include "Color32.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

// hardcoded primitive types
enum PRIMITIVE_TYPE
{
	CUBE,
	PLANE,
	SPHERE,
};

class Entity;
class Mesh;

class ModelComponent : public IComponent
{
public:
	ModelComponent();
	~ModelComponent();

	// creates a primitive model
	void InitPrimitive(PRIMITIVE_TYPE primitive, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"", wchar_t* emissiveMap = L"", float tiling = 1.0f);

	// creates a model from file
	void InitModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"", wchar_t* emissiveMap = L"", bool useMaterial = true, float tiling = 1.0f);
	void Update(const float& delta);

	// overides and calls base
	// also removes/adds all meshes to renderer
	void SetActive(bool) override;

	// set render flags on all meshes in this model
	void SetRenderFlags(unsigned int flags); 

	// get mesh list and how many meshes this model have
	const std::vector<Mesh*>& GetMeshes()    { return _meshes; }
	const unsigned int&       GetNumMeshes() { return _numMeshes; }

	void SetUVDVMap(const wchar_t* texture);
	void SetFoamMap(const wchar_t* texture);
	void SetNoiseMap(const wchar_t* texture);

private:

	void ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling);
	
	// meshes list and num meshes count
	std::vector<Mesh*> _meshes;
	unsigned int       _numMeshes;

	bool _useMaterial;
	
	// render flags
	unsigned int _FLAGS;
};

