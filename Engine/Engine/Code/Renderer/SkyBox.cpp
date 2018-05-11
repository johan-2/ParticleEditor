#include "SkyBox.h"
#include "ShaderManager.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "TransformComponent.h"

#include "DDSTextureLoader/DDSTextureLoader.h"
#include <comdef.h>


SkyBox::SkyBox(wchar_t* textureFile)
{
	CreateBox();
	Loadtexture(textureFile);
}


SkyBox::~SkyBox()
{
}


void SkyBox::CreateBox() 
{
	// allocate memory for vertex and index buffers
	VertexData* vertices = new VertexData[24];
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

	vertices[1].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[1].texture = XMFLOAT2(1, 0);
	vertices[1].normal = XMFLOAT3(0, 0, -1);
	vertices[1].tangent = XMFLOAT3(1, 0, 0);
	vertices[1].binormal = XMFLOAT3(-0, 1, 0);

	vertices[2].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[2].texture = XMFLOAT2(0, 1);
	vertices[2].normal = XMFLOAT3(0, 0, -1);
	vertices[2].tangent = XMFLOAT3(1, 0, 0);
	vertices[2].binormal = XMFLOAT3(-0, 1, 0);

	vertices[3].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[3].texture = XMFLOAT2(1, 1);
	vertices[3].normal = XMFLOAT3(0, 0, -1);
	vertices[3].tangent = XMFLOAT3(1, 0, 0);
	vertices[3].binormal = XMFLOAT3(-0, 1, 0);

	vertices[4].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[4].texture = XMFLOAT2(0, 0);
	vertices[4].normal = XMFLOAT3(1, 0, 0);
	vertices[4].tangent = XMFLOAT3(0, 0, 1);
	vertices[4].binormal = XMFLOAT3(0, 1, 0);

	vertices[5].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[5].texture = XMFLOAT2(1, 0);
	vertices[5].normal = XMFLOAT3(1, 0, 0);
	vertices[5].tangent = XMFLOAT3(0, 0, 1);
	vertices[5].binormal = XMFLOAT3(0, 1, 0);

	vertices[6].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[6].texture = XMFLOAT2(0, 1);
	vertices[6].normal = XMFLOAT3(1, 0, 0);
	vertices[6].tangent = XMFLOAT3(0, 0, 1);
	vertices[6].binormal = XMFLOAT3(0, 1, 0);

	vertices[7].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[7].texture = XMFLOAT2(1, 1);
	vertices[7].normal = XMFLOAT3(1, 0, 0);
	vertices[7].tangent = XMFLOAT3(0, 0, 1);
	vertices[7].binormal = XMFLOAT3(0, 1, 0);

	vertices[8].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[8].texture = XMFLOAT2(0, 0);
	vertices[8].normal = XMFLOAT3(0, 0, 1);
	vertices[8].tangent = XMFLOAT3(-1, 0, 0);
	vertices[8].binormal = XMFLOAT3(0, 1, -0);

	vertices[9].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[9].texture = XMFLOAT2(1, 0);
	vertices[9].normal = XMFLOAT3(0, 0, 1);
	vertices[9].tangent = XMFLOAT3(-1, 0, 0);
	vertices[9].binormal = XMFLOAT3(0, 1, -0);

	vertices[10].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[10].texture = XMFLOAT2(0, 1);
	vertices[10].normal = XMFLOAT3(0, 0, 1);
	vertices[10].tangent = XMFLOAT3(-1, 0, 0);
	vertices[10].binormal = XMFLOAT3(0, 1, -0);

	vertices[11].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[11].texture = XMFLOAT2(1, 1);
	vertices[11].normal = XMFLOAT3(0, 0, 1);
	vertices[11].tangent = XMFLOAT3(-1, 0, 0);
	vertices[11].binormal = XMFLOAT3(0, 1, -0);

	vertices[12].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[12].texture = XMFLOAT2(0, 0);
	vertices[12].normal = XMFLOAT3(-1, 0, 0);
	vertices[12].tangent = XMFLOAT3(0, 0, -1);
	vertices[12].binormal = XMFLOAT3(0, 1, 0);

	vertices[13].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[13].texture = XMFLOAT2(1, 0);
	vertices[13].normal = XMFLOAT3(-1, 0, 0);
	vertices[13].tangent = XMFLOAT3(0, 0, -1);
	vertices[13].binormal = XMFLOAT3(0, 1, 0);

	vertices[14].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[14].texture = XMFLOAT2(0, 1);
	vertices[14].normal = XMFLOAT3(-1, 0, 0);
	vertices[14].tangent = XMFLOAT3(0, 0, -1);
	vertices[14].binormal = XMFLOAT3(0, 1, 0);

	vertices[15].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[15].texture = XMFLOAT2(1, 1);
	vertices[15].normal = XMFLOAT3(-1, 0, 0);
	vertices[15].tangent = XMFLOAT3(0, 0, -1);
	vertices[15].binormal = XMFLOAT3(0, 1, 0);

	vertices[16].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[16].texture = XMFLOAT2(0, 0);
	vertices[16].normal = XMFLOAT3(0, 1, 0);
	vertices[16].tangent = XMFLOAT3(1, 0, 0);
	vertices[16].binormal = XMFLOAT3(0, 0, 1);

	vertices[17].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[17].texture = XMFLOAT2(1, 0);
	vertices[17].normal = XMFLOAT3(0, 1, 0);
	vertices[17].tangent = XMFLOAT3(1, 0, 0);
	vertices[17].binormal = XMFLOAT3(0, 0, 1);

	vertices[18].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[18].texture = XMFLOAT2(0, 1);
	vertices[18].normal = XMFLOAT3(0, 1, 0);
	vertices[18].tangent = XMFLOAT3(1, 0, 0);
	vertices[18].binormal = XMFLOAT3(0, 0, 1);

	vertices[19].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[19].texture = XMFLOAT2(1, 1);
	vertices[19].normal = XMFLOAT3(0, 1, 0);
	vertices[19].tangent = XMFLOAT3(1, 0, 0);
	vertices[19].binormal = XMFLOAT3(0, 0, 1);

	vertices[20].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[20].texture = XMFLOAT2(0, 0);
	vertices[20].normal = XMFLOAT3(0, -1, 0);
	vertices[20].tangent = XMFLOAT3(1, 0, 0);
	vertices[20].binormal = XMFLOAT3(0, 0, -1);

	vertices[21].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[21].texture = XMFLOAT2(1, 0);
	vertices[21].normal = XMFLOAT3(0, -1, 0);
	vertices[21].tangent = XMFLOAT3(1, 0, 0);
	vertices[21].binormal = XMFLOAT3(0, 0, -1);

	vertices[22].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[22].texture = XMFLOAT2(0, 1);
	vertices[22].normal = XMFLOAT3(0, -1, 0);
	vertices[22].tangent = XMFLOAT3(1, 0, 0);
	vertices[22].binormal = XMFLOAT3(0, 0, -1);

	vertices[23].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[23].texture = XMFLOAT2(1, 1);
	vertices[23].normal = XMFLOAT3(0, -1, 0);
	vertices[23].tangent = XMFLOAT3(1, 0, 0);
	vertices[23].binormal = XMFLOAT3(0, 0, -1);

	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	// create the descriptions and rasourcedata to buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 36;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
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

	delete[] vertices;
	delete[] indices;
}

void SkyBox::Loadtexture(wchar_t* file) 
{
	HRESULT result;

	HRESULT result = DirectX::CreateDDSTextureFromFile(DXManager::GetInstance().GetDevice(), file, NULL, &_texture);
	if (FAILED(result))
	{
		_com_error err(result);
		LPCTSTR msg = err.ErrorMessage();
		printf(" %s \n", msg);
		printf("failed to create texture cubemapr\n");		
	}		
}

void SkyBox::Update() 
{
	XMFLOAT3 position = CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionVal();	
	XMFLOAT4X4 matrixPosition;

	XMStoreFloat4x4(&matrixPosition, XMMatrixIdentity());
	XMStoreFloat4x4(&matrixPosition, XMMatrixTranslationFromVector(XMLoadFloat3(&position)));

	_worldMatrix = matrixPosition;
}

void SkyBox::Render() 
{
	Update();

	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	devCon->PSSetShaderResources(0, 1, &_texture);

	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexData);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered, the vertices is sent to the pipeline before the go trought the shader for rendering
	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	ShaderManager::GetInstance().RenderSkyBox(_worldMatrix);

}