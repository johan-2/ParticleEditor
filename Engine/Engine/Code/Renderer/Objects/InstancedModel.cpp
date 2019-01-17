#include "InstancedModel.h"
#include "ModelLoader.h"
#include "DXManager.h"
#include "DXErrorHandler.h"
#include "Systems.h"
#include "Mesh.h"

InstancedModel::InstancedModel(char* model, unsigned int flags, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling)
{
	_FLAGS       = flags;
	_useMaterial = useMaterial;

	//get assimp imoprter
	Assimp::Importer importer;

	// get the scene object from the file
	const aiScene* scene = importer.ReadFile(model, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);

	// assert if scene failed to be created
	assert(scene != nullptr);

	// send the root node and recurivly create all meshes
	ProcessNode(scene->mRootNode, scene, diffuseMap, normalMap, specularMap, emissiveMap, useMaterial, tiling);

	// get how many meshes that was loaded
	_numMeshes = _meshes.size();

	AddToRenderQueues(true);
}

InstancedModel::~InstancedModel()
{
}

void InstancedModel::BuildInstanceBuffer()
{
	if (_instanceBuffer == nullptr)
	{
		// create the descriptions and resource data to buffers
		D3D11_BUFFER_DESC instanceBufferDesc;
		D3D11_SUBRESOURCE_DATA instanceData;

		// Set up the description of the instance buffer.
		instanceBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.ByteWidth           = sizeof(InstanceType) * _instances.size();
		instanceBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags           = 0;
		instanceBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the instance data.
		instanceData.pSysMem          = &_instances[0];
		instanceData.SysMemPitch      = 0;
		instanceData.SysMemSlicePitch = 0;

		HRESULT result = Systems::dxManager->GetDevice()->CreateBuffer(&instanceBufferDesc, &instanceData, &_instanceBuffer);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create instance buffer for instanced model");

		return;
	}
	
	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();
	D3D11_MAPPED_SUBRESOURCE data;

	// map instancebuffer
	HRESULT result = devCon->Map(_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to map instance buffer for instanced model");

	// copy the instancedata over to the instancebuffer
	memcpy(data.pData, (void*)&_instances, sizeof(InstanceType) * _instances.size());

	//unmap
	devCon->Unmap(_instanceBuffer, 0);
}

void InstancedModel::RenderInstances()
{
	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();

	unsigned int strides[2];
	unsigned int offsets[2];
	
	// Set vertex buffer stride and offset.
	strides[0] = sizeof(Mesh::VertexData);
	strides[1] = sizeof(InstanceType);

	offsets[0] = 0;
	offsets[1] = 0;

	// set the instance buffer in slot 1
	devCon->IASetVertexBuffers(1, 1, &_instanceBuffer, &strides[1], &offsets[1]);

	for (int i = 0; i < _numMeshes; i++)
	{
		ID3D11Buffer* vertexBuffer = _meshes[i]->GetVertexBuffer();

		// Set the vertex buffer in slot 0
		devCon->IASetVertexBuffers(0, 1, &vertexBuffer, &strides[0], &offsets[0]);

		// Set the index buffer 
		devCon->IASetIndexBuffer(_meshes[i]->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		// get textures
		ID3D11ShaderResourceView** texture = _meshes[i]->GetTextureArray();

		// set texture
		devCon->PSSetShaderResources(0, 4, texture);

		// draw all instances of this mesh
		devCon->DrawIndexedInstanced(_meshes[i]->GetNumIndices(), _instances.size(), 0, 0, 0);
	}
}

void InstancedModel::AddInstance(XMFLOAT4X4 worldMatrix)
{
	_instances.emplace_back(InstanceType(worldMatrix));
}

void InstancedModel::ProcessNode(aiNode* node, const aiScene* scene, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap, wchar_t* emissiveMap, bool useMaterial, float tiling)
{
	// get and create all meshes in this node
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ModelLoader::CreateMesh(mesh, scene, 0, diffuseMap, normalMap, specularMap, emissiveMap, _useMaterial, tiling, nullptr));
	}

	// recursivly loop over and process all child nodes
	for (UINT i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene, diffuseMap, normalMap, specularMap, emissiveMap, useMaterial, tiling);
}

void InstancedModel::AddToRenderQueues(bool add)
{
	Renderer& renderer = *Systems::renderer;

	if ((_FLAGS & INSTANCED_OPAQUE) == INSTANCED_OPAQUE)
		add ? renderer.AddInstancedModelToRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_DEFERRED) : renderer.RemoveInstancedModelFromRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_DEFERRED);

	if ((_FLAGS & INSTANCED_CAST_SHADOW_DIR) == INSTANCED_CAST_SHADOW_DIR)
		add ? renderer.AddInstancedModelToRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_DEPTH) : renderer.RemoveInstancedModelFromRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_DEPTH);

	if ((_FLAGS & INSTANCED_CAST_REFLECTION) == INSTANCED_CAST_REFLECTION)
		add ? renderer.AddInstancedModelToRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_CAST_REFLECTION) : renderer.RemoveInstancedModelFromRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_CAST_REFLECTION);

	if ((_FLAGS & INSTANCED_REFRACT) == INSTANCED_REFRACT)
		add ? renderer.AddInstancedModelToRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_REFRACT) : renderer.RemoveInstancedModelFromRenderer(this, INSTANCED_SHADER_TYPE::S_INSTANCED_REFRACT);
}