#pragma once

#include <d3d11.h>
#include "IComponent.h"

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

	void InitPrimitive(PRIMITIVE_TYPE primitive, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"");
	void InitModel(char* model, unsigned int flags, wchar_t* diffuseMap = L"", wchar_t* normalMap = L"", wchar_t* specularMap = L"");
	void Update();

	Mesh*& GetMesh() { return _mesh; }

private:

	void CrateCube();
	void CreatePlane();

	Mesh* _mesh;
	

};

