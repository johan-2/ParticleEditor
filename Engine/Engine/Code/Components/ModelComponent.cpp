#include "ModelComponent.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "ModelLoader.h"

using namespace DirectX;

ModelComponent::ModelComponent() : IComponent(COMPONENT_TYPE::MODEL_COMPONENT)
{
}

ModelComponent::~ModelComponent()
{
	for (int i = 0; i < _numMeshes; i++)
		delete _meshes[i];
}

void ModelComponent::InitModel(char* model, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling)
{
	_FLAGS = flags;
	_useMaterial = useMaterial;

	//get assimp imoprter
	Assimp::Importer importer;

	// get the scene object from the file
	const aiScene* scene = importer.ReadFile(model, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	// assert if scene failed to be created
	assert(scene != nullptr, "Failed to load aiScene from file");

	// send the root node and recurivly create all meshes
	ProcessNode(scene->mRootNode, scene, diffuseMap, normalMap, specularMap, emissiveMap, useMaterial, tiling);

	// get how many meshes that was loaded
	_numMeshes = _meshes.size();
}

void ModelComponent::InitPrimitive(PRIMITIVE_TYPE primitive, unsigned int flags, wchar_t* diffuseMap , wchar_t* normalMap , wchar_t* specularMap, wchar_t* emissiveMap, float tiling)
{
	// all primitives only have one mesh
	_numMeshes = 1;
	_FLAGS     = flags;

	if      (primitive == PRIMITIVE_TYPE::CUBE)   _meshes.push_back(ModelLoader::CreateCube(flags, diffuseMap, normalMap, specularMap, emissiveMap, tiling, _parent));
	else if (primitive == PRIMITIVE_TYPE::PLANE)  _meshes.push_back(ModelLoader::CreatePlane(flags, diffuseMap, normalMap, specularMap, emissiveMap, tiling, _parent));
	else if (primitive == PRIMITIVE_TYPE::SPHERE) _meshes.push_back(ModelLoader::CreateSphere(flags, diffuseMap, normalMap, specularMap, emissiveMap, tiling, _parent));
}

void ModelComponent::InitGrid(unsigned int size, float cellSize, Color32 gridColor, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, float tiling)
{	
	// grid only use one mesh
	_numMeshes = 1;
	_FLAGS     = flags;

	_meshes.push_back(ModelLoader::CreateGrid(size, cellSize, gridColor, flags, diffuseMap, normalMap, specularMap, emissiveMap, tiling, _parent));
}

void ModelComponent::Update(const float& delta)
{	
}

void ModelComponent::SetActive(bool active) 
{
	// return if the state we are trying to set is the 
	// same that we already is in
	if (active == _isActive)
		return;

	// set active flag in component base
	IComponent::SetActive(active);
	
	// remove/add meshes to renderer
	for (int i = 0; i < _meshes.size(); i++)
		_meshes[i]->AddRemoveToRenderer(active);
}

// set the flag on all meshes in this model
void ModelComponent::SetRenderFlags(unsigned int flags)
{ 
	// do nothing if the component is inactive or if we are trying to set the same flags we already have
	if (!_isActive || (_FLAGS & flags) == _FLAGS)
		return;

	// set the new flags
	_FLAGS = flags;

	// remove our meshes from the renderer
	// set new render flags and re add them to the renderer
	for (int i = 0; i < _numMeshes; i++)
	{
		_meshes[i]->AddRemoveToRenderer(false);
		_meshes[i]->SetFlags(flags);
		_meshes[i]->AddRemoveToRenderer(true);
	}
}

void ModelComponent::ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling)
{
	// get and create all meshes in this node
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ModelLoader::CreateMesh(mesh, scene, _FLAGS, diffuseMap, normalMap, specularMap, emissiveMap, _useMaterial, tiling, _parent));
	}

	// recursivly loop over and process all child nodes
	for (UINT i = 0; i < node->mNumChildren; i++)	
		ProcessNode(node->mChildren[i], scene, diffuseMap, normalMap, specularMap, emissiveMap, useMaterial, tiling);	
}




