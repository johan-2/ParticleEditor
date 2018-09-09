#include "ModelComponent.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"

using namespace DirectX;

ModelComponent::ModelComponent() : IComponent(COMPONENT_TYPE::MODEL_COMPONENT)
{

}

void ModelComponent::InitPrimitive(PRIMITIVE_TYPE primitive, unsigned int flags, wchar_t* diffuseMap , wchar_t* normalMap , wchar_t* specularMap )
{
	_mesh = new Mesh(_parent, flags, diffuseMap, normalMap, specularMap);

	switch (primitive)
	{

	case PRIMITIVE_TYPE::CUBE:
		CrateCube();
		break;
	case PRIMITIVE_TYPE::PLANE:
		CreatePlane();
		break;

	default:
		break;
	}
}

void ModelComponent::InitModel(char* model, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap)
{
	_mesh = new Mesh(_parent, flags, diffuseMap, normalMap, specularMap);

	//TODO : IMPLEMENT MODEL LOADING
}

ModelComponent::~ModelComponent()
{
	delete _mesh;
}

void ModelComponent::CrateCube() 
{
	// allocate memory for vertex and index buffers
	Mesh::VertexData* vertices = new Mesh::VertexData[24];
	unsigned long* indices = new unsigned long[36]
	{
		0,1,2,2,1,3,
		4,5,6,6,5,7,
		8,9,10,10,9,11,
		12,13,14,14,13,15,
		16,17,18,18,17,19,
		20,21,22,22,21,23

	};

	vertices[0].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[0].texture = XMFLOAT2(0, 0);
	vertices[0].normal = XMFLOAT3(0, 0, -1);
	vertices[0].tangent = XMFLOAT3(1, 0, 0);
	vertices[0].binormal = XMFLOAT3(-0, 1, 0);
	vertices[0].color = Color32(255, 255, 255, 255);

	vertices[1].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[1].texture = XMFLOAT2(1, 0);
	vertices[1].normal = XMFLOAT3(0, 0, -1);
	vertices[1].tangent = XMFLOAT3(1, 0, 0);
	vertices[1].binormal = XMFLOAT3(-0, 1, 0);
	vertices[1].color = Color32(255, 255, 255, 255);

	vertices[2].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[2].texture = XMFLOAT2(0, 1);
	vertices[2].normal = XMFLOAT3(0, 0, -1);
	vertices[2].tangent = XMFLOAT3(1, 0, 0);
	vertices[2].binormal = XMFLOAT3(-0, 1, 0);
	vertices[2].color = Color32(255, 255, 255, 255);

	vertices[3].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[3].texture = XMFLOAT2(1, 1);
	vertices[3].normal = XMFLOAT3(0, 0, -1);
	vertices[3].tangent = XMFLOAT3(1, 0, 0);
	vertices[3].binormal = XMFLOAT3(-0, 1, 0);
	vertices[3].color = Color32(255, 255, 255, 255);

	vertices[4].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[4].texture = XMFLOAT2(0, 0);
	vertices[4].normal = XMFLOAT3(1, 0, 0);
	vertices[4].tangent = XMFLOAT3(0, 0, 1);
	vertices[4].binormal = XMFLOAT3(0, 1, 0);
	vertices[4].color = Color32(255, 255, 255, 255);

	vertices[5].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[5].texture = XMFLOAT2(1, 0);
	vertices[5].normal = XMFLOAT3(1, 0, 0);
	vertices[5].tangent = XMFLOAT3(0, 0, 1);
	vertices[5].binormal = XMFLOAT3(0, 1, 0);
	vertices[5].color = Color32(255, 255, 255, 255);

	vertices[6].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[6].texture = XMFLOAT2(0, 1);
	vertices[6].normal = XMFLOAT3(1, 0, 0);
	vertices[6].tangent = XMFLOAT3(0, 0, 1);
	vertices[6].binormal = XMFLOAT3(0, 1, 0);
	vertices[6].color = Color32(255, 255, 255, 255);

	vertices[7].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[7].texture = XMFLOAT2(1, 1);
	vertices[7].normal = XMFLOAT3(1, 0, 0);
	vertices[7].tangent = XMFLOAT3(0, 0, 1);
	vertices[7].binormal = XMFLOAT3(0, 1, 0);
	vertices[7].color = Color32(255, 255, 255, 255);

	vertices[8].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[8].texture = XMFLOAT2(0, 0);
	vertices[8].normal = XMFLOAT3(0, 0, 1);
	vertices[8].tangent = XMFLOAT3(-1, 0, 0);
	vertices[8].binormal = XMFLOAT3(0, 1, -0);
	vertices[8].color = Color32(255, 255, 255, 255);

	vertices[9].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[9].texture = XMFLOAT2(1, 0);
	vertices[9].normal = XMFLOAT3(0, 0, 1);
	vertices[9].tangent = XMFLOAT3(-1, 0, 0);
	vertices[9].binormal = XMFLOAT3(0, 1, -0);
	vertices[9].color = Color32(255, 255, 255, 255);

	vertices[10].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[10].texture = XMFLOAT2(0, 1);
	vertices[10].normal = XMFLOAT3(0, 0, 1);
	vertices[10].tangent = XMFLOAT3(-1, 0, 0);
	vertices[10].binormal = XMFLOAT3(0, 1, -0);
	vertices[10].color = Color32(255, 255, 255, 255);

	vertices[11].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[11].texture = XMFLOAT2(1, 1);
	vertices[11].normal = XMFLOAT3(0, 0, 1);
	vertices[11].tangent = XMFLOAT3(-1, 0, 0);
	vertices[11].binormal = XMFLOAT3(0, 1, -0);
	vertices[11].color = Color32(255, 255, 255, 255);

	vertices[12].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[12].texture = XMFLOAT2(0, 0);
	vertices[12].normal = XMFLOAT3(-1, 0, 0);
	vertices[12].tangent = XMFLOAT3(0, 0, -1);
	vertices[12].binormal = XMFLOAT3(0, 1, 0);
	vertices[12].color = Color32(255, 255, 255, 255);

	vertices[13].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[13].texture = XMFLOAT2(1, 0);
	vertices[13].normal = XMFLOAT3(-1, 0, 0);
	vertices[13].tangent = XMFLOAT3(0, 0, -1);
	vertices[13].binormal = XMFLOAT3(0, 1, 0);
	vertices[13].color = Color32(255, 255, 255, 255);

	vertices[14].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[14].texture = XMFLOAT2(0, 1);
	vertices[14].normal = XMFLOAT3(-1, 0, 0);
	vertices[14].tangent = XMFLOAT3(0, 0, -1);
	vertices[14].binormal = XMFLOAT3(0, 1, 0);
	vertices[14].color = Color32(255, 255, 255, 255);

	vertices[15].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[15].texture = XMFLOAT2(1, 1);
	vertices[15].normal = XMFLOAT3(-1, 0, 0);
	vertices[15].tangent = XMFLOAT3(0, 0, -1);
	vertices[15].binormal = XMFLOAT3(0, 1, 0);
	vertices[15].color = Color32(255, 255, 255, 255);

	vertices[16].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[16].texture = XMFLOAT2(0, 0);
	vertices[16].normal = XMFLOAT3(0, 1, 0);
	vertices[16].tangent = XMFLOAT3(1, 0, 0);
	vertices[16].binormal = XMFLOAT3(0, 0, 1);
	vertices[16].color = Color32(255, 255, 255, 255);

	vertices[17].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[17].texture = XMFLOAT2(1, 0);
	vertices[17].normal = XMFLOAT3(0, 1, 0);
	vertices[17].tangent = XMFLOAT3(1, 0, 0);
	vertices[17].binormal = XMFLOAT3(0, 0, 1);
	vertices[17].color = Color32(255, 255, 255, 255);

	vertices[18].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[18].texture = XMFLOAT2(0, 1);
	vertices[18].normal = XMFLOAT3(0, 1, 0);
	vertices[18].tangent = XMFLOAT3(1, 0, 0);
	vertices[18].binormal = XMFLOAT3(0, 0, 1);
	vertices[18].color = Color32(255, 255, 255, 255);

	vertices[19].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[19].texture = XMFLOAT2(1, 1);
	vertices[19].normal = XMFLOAT3(0, 1, 0);
	vertices[19].tangent = XMFLOAT3(1, 0, 0);
	vertices[19].binormal = XMFLOAT3(0, 0, 1);
	vertices[19].color = Color32(255, 255, 255, 255);

	vertices[20].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[20].texture = XMFLOAT2(0, 0);
	vertices[20].normal = XMFLOAT3(0, -1, 0);
	vertices[20].tangent = XMFLOAT3(1, 0, 0);
	vertices[20].binormal = XMFLOAT3(0, 0, -1);
	vertices[20].color = Color32(255, 255, 255, 255);

	vertices[21].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[21].texture = XMFLOAT2(1, 0);
	vertices[21].normal = XMFLOAT3(0, -1, 0);
	vertices[21].tangent = XMFLOAT3(1, 0, 0);
	vertices[21].binormal = XMFLOAT3(0, 0, -1);
	vertices[21].color = Color32(255, 255, 255, 255);

	vertices[22].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[22].texture = XMFLOAT2(0, 1);
	vertices[22].normal = XMFLOAT3(0, -1, 0);
	vertices[22].tangent = XMFLOAT3(1, 0, 0);
	vertices[22].binormal = XMFLOAT3(0, 0, -1);
	vertices[22].color = Color32(255, 255, 255, 255);

	vertices[23].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[23].texture = XMFLOAT2(1, 1);
	vertices[23].normal = XMFLOAT3(0, -1, 0);
	vertices[23].tangent = XMFLOAT3(1, 0, 0);
	vertices[23].binormal = XMFLOAT3(0, 0, -1);
	vertices[23].color = Color32(255, 255, 255, 255);
	
	// create mesh and buffers	
	_mesh->CreateBuffers(vertices, indices, 24, 36);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	delete[] indices;
}

void ModelComponent::CreatePlane()
{
	Mesh::VertexData* vertices = new Mesh::VertexData[4];
	vertices[0].position = XMFLOAT3(-2.0f, 0.0f, 2.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[0].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[0].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[0].color = Color32(255, 255, 255, 255);

	vertices[1].position = XMFLOAT3(2.0f, 0.0f, 2.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[1].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[1].color = Color32(255, 255, 255, 255);

	vertices[2].position = XMFLOAT3(-2.0f, 0.0f, -2.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[2].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[2].color = Color32(255, 255, 255, 255);

	vertices[3].position = XMFLOAT3(2.0f, 0.0f, -2.0f);
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[3].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);
	vertices[3].binormal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	vertices[3].color = Color32(255, 255, 255, 255);

	unsigned long* indices = new unsigned long[6]{ 0,1,2,2,1,3 };

	_mesh->CreateBuffers(vertices, indices, 4, 6);

	delete[] vertices;
	delete[] indices;
}

void ModelComponent::Update() 
{
	
}


