#pragma once
#include <d3d11.h>
#include "IComponent.h"
#include <vector>
#include "Color32.h"

// hardcoded primitive types
enum PRIMITIVE_TYPE
{
	CUBE,
	PLANE
};

class Entity;
class Mesh;

class ModelComponent : public IComponent
{
public:
	ModelComponent();
	~ModelComponent();

	// creates a primitive model
	void InitPrimitive(PRIMITIVE_TYPE primitive, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"");

	// creates a 2D grid along x and y axis
	void InitGrid(unsigned int size, float cellSize, Color32 gridColor);

	// creates a model from file
	void InitModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"");
	void Update();

	void SetActive(bool) override;

	// get mesh list and how many meshes this model have
	const std::vector<Mesh*>& GetMeshes()    { return _meshes; }
	const unsigned int&       GetNumMeshes() { return _numMeshes; }

private:

	// create primitives
	void CrateCube(unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap);
	void CreatePlane(unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap);
	
	// meshes list and num meshes count
	std::vector<Mesh*> _meshes;
	unsigned int      _numMeshes;

};

