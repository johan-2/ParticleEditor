#include "Mesh.h"
#include "DXManager.h"
#include "Renderer.h"
#include "TexturePool.h"


Mesh::Mesh(Entity* parent, unsigned int FLAGS, wchar_t* diffuseMap, wchar_t* normalMap, wchar_t* specularMap ) :
	_uvOffset(XMFLOAT2(0,0)),
	_FLAGS(0)
{
	_FLAGS = FLAGS;

	_transform = parent->GetComponent<TransformComponent>();

	TexturePool& TP = TexturePool::GetInstance();
	diffuseMap  != L"" ? _textures[0] = TP.GetTexture(diffuseMap)  : _textures[0] = TP.GetTexture(L"Textures/defaultDiffuse.dds");
	normalMap   != L"" ? _textures[1] = TP.GetTexture(normalMap)   : _textures[1] = TP.GetTexture(L"Textures/defaultNormal.dds");
	specularMap != L"" ? _textures[2] = TP.GetTexture(specularMap) : _textures[2] = TP.GetTexture(L"Textures/defaultSpecular.dds");

	AddRemoveToRenderer(true);
}

Mesh::~Mesh()
{	
	_vertexBuffer->Release();
	_indexBuffer->Release();

	AddRemoveToRenderer(false);
}

void Mesh::CreateBuffers(VertexData* verticesData, unsigned long* indicesData, unsigned int numVertices, unsigned int numIndices) 
{
	// save number of vertices/indices
	_numVertices = numVertices;
	_numIndices = numIndices;

	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	// create the descriptions and rasourcedata to buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * numVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * numIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = verticesData;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indicesData;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result;

	// create vertex and index buffers
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
		printf("failed to create vertexbuffer for cube\n");

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
		printf("failed to create indexbuffer for cube\n");

}

void Mesh::UploadBuffers() 
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexData);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered, the vertices is sent to the pipeline before the go trought the shader for rendering
	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::AddRemoveToRenderer(bool add)
{
	Renderer& renderer = Renderer::GetInstance();

	if ((_FLAGS & HAS_ALPHA) == HAS_ALPHA) 
	{
		if ((_FLAGS & CAST_SHADOW_DIR) == CAST_SHADOW_DIR || (_FLAGS & LIGHTS_ALL) == LIGHTS_ALL)
			add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DEPTH) : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DEPTH);

		add ? renderer.AddToAlphaMeshes(this) : renderer.RemoveFromAlphaMeshes(this);	
		return;
	}

    if ((_FLAGS & AMBIENT) == AMBIENT)
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_AMBIENT)             : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_AMBIENT);

	if ((_FLAGS & DIRECTIONAL) == DIRECTIONAL)
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DIRECTIONAL)         : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DIRECTIONAL);

	if ((_FLAGS & CAST_SHADOW_DIR) == CAST_SHADOW_DIR)
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DEPTH)               : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DEPTH);

	if ((_FLAGS & RECIVE_SHADOW_DIR) == RECIVE_SHADOW_DIR) 
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DIRECTIONAL_SHADOWS) : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DIRECTIONAL_SHADOWS);

	if ((_FLAGS & POINT) == POINT)
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_POINT)               : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_POINT);
	

	if ((_FLAGS & LIGHTS_ALL) == LIGHTS_ALL)
	{
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_AMBIENT)             : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_AMBIENT);
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DEPTH)               : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DEPTH);
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_DIRECTIONAL_SHADOWS) : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_DIRECTIONAL_SHADOWS);
		add ? renderer.AddToRenderer(this, SHADER_TYPE::S_POINT)               : renderer.RemoveFromRenderer(this, SHADER_TYPE::S_POINT);
	}

	
}

